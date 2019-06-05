package com.sandbox;

import android.util.Base64;
import com.GHL.Log;

import android.content.Context;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.content.Intent;
import android.content.IntentSender.SendIntentException;

import android.content.pm.ResolveInfo;

import android.app.PendingIntent;

import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClient.BillingResponse;
import com.android.billingclient.api.BillingClient.FeatureType;
import com.android.billingclient.api.BillingClient.SkuType;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.Purchase.PurchasesResult;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;

import android.text.TextUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;


import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.Signature;
import java.security.SignatureException;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.X509EncodedKeySpec;

public class IAPHelper implements PurchasesUpdatedListener {

    private Activity m_activity;
    private String m_public_key;
    private long m_native_object;

    private BillingClient m_client;
    private boolean m_is_service_connected;
   
    private final Map<String,Purchase> m_purchase_map = new HashMap<String,Purchase>();
    private final Map<String,SkuDetails> m_products_map = new HashMap<String,SkuDetails>();
    private final Set<String> m_tokens_to_be_consumed = new HashSet<String>();
    private String m_pending_purchase_sku;


    private static final String TAG = "IAP";
    
    public  IAPHelper(Activity activity,long obj) {
        m_activity = activity;
        m_native_object = obj;
        m_client = BillingClient.newBuilder(m_activity).setListener(this).build();

        Log.d(TAG, "Starting setup.");
        startServiceConnection(new Runnable() {
            @Override
            public void run() {
                IAPHelper.this.onBillingClientSetupFinished();
            }
        });
    }




    public void destroy() {
        Log.d(TAG, "Destroying the helper.");

        if (m_client != null && m_client.isReady()) {
            m_client.endConnection();
            m_client = null;
        }
    }

    public SkuDetails getSkuDetails(String sku) {
        return m_products_map.get(sku);
    }

    public Activity getActivity() {
        return m_activity;
    }

    public boolean is_setup_done() {
        return m_is_service_connected;
    }

    public boolean is_subscriptions_supported() {
        int responseCode = m_client.isFeatureSupported(FeatureType.SUBSCRIPTIONS);
        if (responseCode != BillingResponse.OK) {
            Log.w(TAG, "areSubscriptionsSupported() got an error response: " + responseCode);
        }
        return responseCode == BillingResponse.OK;
    }

    @Override
    public void onPurchasesUpdated(int resultCode, List<Purchase> purchases) {
        Log.d(TAG,"onPurchasesUpdated: " + resultCode + " purchases: " + ( purchases==null ? "null" : "yes"));
        if (resultCode == BillingResponse.OK) {
            if (purchases != null) {
                for (Purchase purchase:purchases) {
                    handlePurchase(purchase);
                }
                onPurchasesUpdated();
            } else {
                queryPurchases();
            }
        } else if (resultCode == BillingResponse.USER_CANCELED) {
            Log.i(TAG, "onPurchasesUpdated() - user cancelled the purchase flow - skipping");
        } else {
            Log.w(TAG, "onPurchasesUpdated() got unknown resultCode: " + resultCode);
        }
        if (resultCode != BillingResponse.OK && 
            m_pending_purchase_sku != null) {
            Log.d(TAG,"report failed purchase");
        
            on_iap_purchase_finished(m_pending_purchase_sku,
                            resultCode,null);
            m_pending_purchase_sku = null;
        }
    }        

    private String getTransaction(Purchase purchase) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put("product_id",purchase.getSku());
        obj.put("id",purchase.getOrderId());
        obj.put("at",purchase.getPurchaseTime()/1000);
        obj.put("ticket",purchase.getOriginalJson());
        obj.put("signature",purchase.getSignature());
        return obj.toString();
    }

    private void onBillingClientSetupFinished() {
        Log.d(TAG, "Setup successful. Querying inventory.");
        nativeProcessResponse(m_native_object,"iap_init","{\"status\":\"success\"}");
    }

    private void onPurchasesUpdated() {
        Log.d(TAG, "Purchases updated.");
        String skip_restore = null;
        if (m_pending_purchase_sku != null) {
            Log.d(TAG,"Has pending purchase sku: " + m_pending_purchase_sku);
            for (Purchase purchase: m_purchase_map.values()) {
                Log.d(TAG,"Process purchase for sku: " + purchase.getSku());
                if (purchase.getSku().equals(m_pending_purchase_sku)) {
                    Log.d(TAG,"Found purchase for pending sku");
                    on_iap_purchase_finished(m_pending_purchase_sku,
                        BillingResponse.OK,purchase);
                    skip_restore = m_pending_purchase_sku;
                    m_pending_purchase_sku = null;
                    break;
                }
            }
        }

        for (Purchase purchase: m_purchase_map.values()) {
            try {
                if (skip_restore!= null && 
                    skip_restore.equals(purchase.getSku())) {
                    continue;
                }
                JSONObject obj = new JSONObject();
                obj.put("product_id", purchase.getSku());
                obj.put("token",purchase.getPurchaseToken());
                obj.put("state", "restored");
                obj.put("transaction", getTransaction(purchase));
                nativeProcessResponse(m_native_object, "iap_restore_payments", obj.toString());
            } catch ( JSONException e) {
                Log.e(TAG,e.toString());
            }
        }
    }

    public boolean iap_purchase(final String data) {
        try {
            JSONObject obj = new JSONObject(data);
            String sku = obj.optString("product_id");
            if (sku == null) {
                Log.e(TAG,"need product_id");
                return false;
            }
            String type = obj.optString("type",SkuType.INAPP);
            initiatePurchaseFlow(sku, type);
            return true;
        } catch ( JSONException e) {
            Log.w(TAG,e.toString());
        }
        initiatePurchaseFlow(data,  SkuType.INAPP);
        return true;
    }
    
    public boolean iap_get_items_info(final String json_list) {
        try {
            JSONArray jsonArray = new JSONArray(json_list);
            ArrayList<String> stringArray = new ArrayList<String>();
            for(int i = 0, count = jsonArray.length(); i< count; i++)
            {
                stringArray.add(jsonArray.getString(i));
            }
            on_query_products(SkuType.INAPP,stringArray);
            return true;
        }  catch ( JSONException e) {
            Log.e(TAG,e.toString());
        }
        return false;
    }
    public boolean iap_get_subscriptions_info(final String json_list) {
        try {
            JSONArray jsonArray = new JSONArray(json_list);
            ArrayList<String> stringArray = new ArrayList<String>();
            for(int i = 0, count = jsonArray.length(); i< count; i++)
            {
                stringArray.add(jsonArray.getString(i));
            }
            on_query_products(SkuType.SUBS,stringArray);
            return true;
        }  catch ( JSONException e) {
            Log.e(TAG,e.toString());
        }
        return false;
    }

    /**
     * Start a purchase or subscription replace flow
     */
    public void initiatePurchaseFlow(final String skuId, 
            final @SkuType String billingType) {
        Runnable purchaseFlowRequest = new Runnable() {
            @Override
            public void run() {
                IAPHelper.this.m_pending_purchase_sku = skuId;
                Log.d(TAG, "Launching in-app purchase flow." + 
                    "\n skuId:"+skuId + 
                    "\n billingType:"+billingType);
               
                int res = m_client.launchBillingFlow(m_activity, 
                    BillingFlowParams.newBuilder()
                        .setSku(skuId).setType(billingType)
                        .build() );
                if (res != BillingResponse.OK) {
                    on_iap_purchase_finished(skuId,res,null);
                }
            }
        };

        executeServiceRequest(purchaseFlowRequest);
    }


    protected void on_query_products(@SkuType final String itemType, final List<String> skuList) {
        // Creating a runnable from the request to use it inside our connection retry policy below
        Runnable queryRequest = new Runnable() {
            @Override
            public void run() {
                // Query the purchase async
                SkuDetailsParams.Builder params = SkuDetailsParams.newBuilder();
                params.setSkusList(skuList).setType(itemType);
                m_client.querySkuDetailsAsync(params.build(),
                        new SkuDetailsResponseListener() {
                            @Override
                            public void onSkuDetailsResponse(int responseCode,
                                                             List<SkuDetails> skuDetailsList) {
                                IAPHelper.this.onSkuDetailsResponse(responseCode,skuDetailsList);
                            }
                        });
            }
        };

        executeServiceRequest(queryRequest);
    }
    private void putResult(JSONObject obj, int responseCode) throws JSONException {
        JSONObject res = new JSONObject();
        res.put("code",responseCode);
        res.put("success",responseCode == BillingResponse.OK);
        obj.put("result",res);
    }
    private void onSkuDetailsResponse(int responseCode, List<SkuDetails> skuDetailsList) {
        JSONObject obj = new JSONObject();
        try {
            putResult(obj,responseCode);
             if (responseCode == BillingResponse.OK) {
                if (skuDetailsList != null) {
                    obj.put("products", buildProductsJson(skuDetailsList));
                    for (SkuDetails sku:skuDetailsList) {
                        m_products_map.put(sku.getSku(),sku);
                    }
                }
             } 
        }
        catch (JSONException e) {
           Log.e(TAG,e.toString());
        }
        nativeProcessResponse(m_native_object,"iap_get_products_info",obj.toString());
    }

    public boolean iap_confirm_transaction(final String transaction) {
        JSONObject transactionObj = null;
        try {
           transactionObj = new JSONObject(transaction);
        } catch (JSONException e) {
           Log.e(TAG,e.toString());
           return false;
        }

        Purchase purchase = findPurchase(transactionObj);
        if (purchase == null) {
            Log.i(TAG, "Unknown purchase");
            return false;
        }

        on_consume_purchase(purchase,transactionObj);
        return true;
    }

    public boolean iap_confirm_subscription(final String transaction) {
        JSONObject transactionObj = null;
        try {
           transactionObj = new JSONObject(transaction);
        } catch (JSONException e) {
           Log.e(TAG,e.toString());
           return false;
        }

        Purchase purchase = findPurchase(transactionObj);
        if (purchase == null) {
            Log.i(TAG, "Unknown purchase");
            return false;
        }

        on_consume_subscription(purchase,transactionObj);
        return true;
    }

    protected void on_consume_purchase(Purchase purchase, JSONObject transaction) {
        final String purchaseToken = purchase.getPurchaseToken();
        final String product_id = purchase.getSku();

        // If we've already scheduled to consume this token - no action is needed (this could happen
        // if you received the token when querying purchases inside onReceive() and later from
        // onActivityResult()
        if (m_tokens_to_be_consumed.contains(purchaseToken)) {
            Log.i(TAG, "Token was already scheduled to be consumed - skipping...");
            return;
        }
        m_tokens_to_be_consumed.add(purchaseToken);

        // Generating Consume Response listener
        final ConsumeResponseListener onConsumeListener = new ConsumeResponseListener() {
            @Override
            public void onConsumeResponse(@BillingResponse int responseCode, String purchaseToken) {
                IAPHelper.this.onConsumeFinished(responseCode,purchaseToken,product_id);
            }
        };

        // Creating a runnable from the request to use it inside our connection retry policy below
        Runnable consumeRequest = new Runnable() {
            @Override
            public void run() {
                // Consume the purchase async
                m_client.consumeAsync(purchaseToken, onConsumeListener);
            }
        };

        executeServiceRequest(consumeRequest);
    }

    protected void on_consume_subscription(Purchase purchase, JSONObject transaction) {
        final String purchaseToken = purchase.getPurchaseToken();
        final String product_id = purchase.getSku();
        JSONObject obj = new JSONObject();
        try {
            obj.put("product_id",product_id);
            obj.put("token",purchaseToken);
        } catch (JSONException e) {
           Log.e(TAG,e.toString());
        }
        if (purchaseToken != null) {
	        m_tokens_to_be_consumed.add(purchaseToken);
	        m_purchase_map.remove(purchaseToken);
	    }
        nativeProcessResponse(m_native_object,"iap_confirm_subscription",obj.toString());
    }

    private void onConsumeFinished(@BillingResponse int responseCode, String purchaseToken,String product_id) {
         try {

            Log.d(TAG, "Consumption finished. Purchase token: " + purchaseToken + ", result: " + responseCode);

            if (responseCode == BillingResponse.OK) {
                m_purchase_map.remove(purchaseToken);
            }

            JSONObject obj = new JSONObject();
            obj.put("product_id",product_id);
            obj.put("token",purchaseToken);
            putResult(obj,responseCode);
            nativeProcessResponse(m_native_object,"iap_confirm_transaction",obj.toString());
        } catch (JSONException e) {
            Log.e(TAG, e.toString());
        }
    }

    private void onQueryPurchasesFinished(PurchasesResult result) {
        // Have we been disposed of in the meantime? If so, or bad result code, then quit
        if (m_client == null || result.getResponseCode() != BillingResponse.OK) {
            Log.w(TAG, "Billing client was null or result code (" + result.getResponseCode()
                    + ") was bad - quitting");
            return;
        }

        Log.d(TAG, "Query purchases was successful.");

        // Update the UI and purchases inventory with new list of purchases
        m_purchase_map.clear();
        if (result.getPurchasesList() != null) {
            for (Purchase purchase : result.getPurchasesList()) {
                handlePurchase(purchase);
            }
        }
        onPurchasesUpdated();
    }

    private void queryPurchases() {
        Runnable queryToExecute = new Runnable() {
            @Override
            public void run() {
                try {
                    long time = System.currentTimeMillis();
                    PurchasesResult purchasesResult = m_client.queryPurchases(SkuType.INAPP);
                    Log.i(TAG, "Querying purchases elapsed time: " + (System.currentTimeMillis() - time)
                            + "ms");
                    // If there are subscriptions supported, we add subscription rows as well
                    if (is_subscriptions_supported()) {
                        PurchasesResult subscriptionResult
                                = m_client.queryPurchases(SkuType.SUBS);
                        Log.i(TAG, "Querying purchases and subscriptions elapsed time: "
                                + (System.currentTimeMillis() - time) + "ms");
                        Log.i(TAG, "Querying subscriptions result code: "
                                + subscriptionResult.getResponseCode() );

                        if (subscriptionResult.getResponseCode() == BillingResponse.OK) {
                            if (subscriptionResult.getPurchasesList() != null &&
                                purchasesResult.getPurchasesList() != null) {
                                purchasesResult.getPurchasesList().addAll(
                                    subscriptionResult.getPurchasesList());
                            }
                        } else {
                            Log.e(TAG, "Got an error response trying to query subscription purchases");
                        }
                    } else if (purchasesResult.getResponseCode() == BillingResponse.OK) {
                        Log.i(TAG, "Skipped subscription purchases query since they are not supported");
                    } else {
                        Log.w(TAG, "queryPurchases() got an error response code: "
                                + purchasesResult.getResponseCode());
                    }
                    onQueryPurchasesFinished(purchasesResult);
                } catch (Exception e) {
                    Log.e( TAG, "queryPurchases " + e.toString());
                }
                
            }
        };

        executeServiceRequest(queryToExecute);
    }

    private void handlePurchase(Purchase purchase) {
        if (!verifyPurchase(purchase.getOriginalJson(), purchase.getSignature())) {
            Log.i(TAG, "Got a purchase: " + purchase + "; but signature is bad. Skipping...");
            return;
        }

        Log.d(TAG, "Got a verified purchase: " + purchase);
        String purchaseToken = purchase.getPurchaseToken();
        if (m_tokens_to_be_consumed.contains(purchaseToken)) {
            Log.i(TAG, "Already consumed");
        } else {
            m_purchase_map.put(purchaseToken,purchase);
        }
    }

    private Purchase findPurchase(JSONObject transaction) {
        for (Purchase purchase: m_purchase_map.values()) {
            String ticket = transaction.optString("ticket","");
            String signature = transaction.optString("signature","");
            if (ticket.equals(purchase.getOriginalJson()) && signature.equals(purchase.getSignature())) {
                return purchase;
            }
        }
        return null;
    }


    public boolean iap_init(String key) {
        m_public_key = key;
        return true;
    }

    public boolean iap_restore_payments() {
        queryPurchases();
        return true;
    }


    private JSONArray buildProductsJson(List<SkuDetails> products) {
        JSONArray res = new JSONArray();
        for (SkuDetails sku: products) {
            JSONObject product = new JSONObject();

            try {

                product.put("product_id",sku.getSku());
                product.put("localized_price",sku.getPrice());
                product.put("localized_name",sku.getTitle());
                product.put("localized_descr",sku.getDescription());
                product.put("currency_code",sku.getPriceCurrencyCode());
                product.put("price",sku.getPriceAmountMicros() /  1000000.0);
            } catch ( JSONException e) {
                Log.e(TAG,e.toString());
            }

            res.put(product);
        }
        return res;
    }


    private void startServiceConnection(final Runnable executeOnSuccess) {
        Log.i(TAG,"startServiceConnection");
        m_client.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(@BillingResponse int billingResponseCode) {
                Log.d(TAG, "Setup finished. Response code: " + billingResponseCode);
                if (m_client != null) {
                    Log.i(TAG, "isReady: " + m_client.isReady());

                    if (billingResponseCode == BillingResponse.OK) {
                        m_is_service_connected = true;
                        if (executeOnSuccess != null) {
                            executeOnSuccess.run();
                        }
                    }
                } else {
                    Log.i(TAG, "client destroyed");
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                Log.i(TAG,"onBillingServiceDisconnected");
                m_is_service_connected = false;
                nativeProcessResponse(m_native_object,"iap_init","{\"disconnected\":true}");
            }
        });
    }

    private void executeServiceRequest(Runnable runnable) {
        if (m_is_service_connected) {
            runnable.run();
        } else {
            // If billing service was disconnected, we try to reconnect 1 time.
            // (feel free to introduce your retry policy here).
            startServiceConnection(runnable);
        }
    }


    private static native void nativeProcessResponse(long obj,String method,String data);




    void on_iap_purchase_finished(String sku,int result, Purchase info) {
        try {
            JSONObject obj = new JSONObject();
            obj.put("product_id",sku);
            putResult(obj,result);
            if (info != null) {
                obj.put("transaction",getTransaction(info));
            }
            nativeProcessResponse(m_native_object,"iap_purchase",obj.toString());
        } catch (JSONException e) {
            Log.e(TAG,e.toString());
        }
    }



    private static final String KEY_FACTORY_ALGORITHM = "RSA";
    private static final String SIGNATURE_ALGORITHM = "SHA1withRSA";

    boolean verify(PublicKey publicKey, String signedData, String signature) {
        byte[] signatureBytes;
        try {
            signatureBytes = Base64.decode(signature, Base64.DEFAULT);
        } catch (IllegalArgumentException e) {
            Log.e( TAG, "Base64 decoding failed.");
            return false;
        }
        try {
            Signature sig = Signature.getInstance(SIGNATURE_ALGORITHM);
            sig.initVerify(publicKey);
            sig.update(signedData.getBytes());
            if (!sig.verify(signatureBytes)) {
                Log.e( TAG, "Signature verification failed.");
                return false;
            }
            return true;
        } catch (NoSuchAlgorithmException e) {
            Log.e( TAG, "NoSuchAlgorithmException.");
        } catch (InvalidKeyException e) {
            Log.e( TAG, "Invalid key specification.");
        } catch (SignatureException e) {
            Log.e( TAG,  "Signature exception.");
        }
        return false;
    }





    PublicKey generatePublicKey(String encodedPublicKey) {
        try {
            byte[] decodedKey = Base64.decode(encodedPublicKey, Base64.DEFAULT);
            KeyFactory keyFactory = KeyFactory.getInstance(KEY_FACTORY_ALGORITHM);
            return keyFactory.generatePublic(new X509EncodedKeySpec(decodedKey));
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        } catch (InvalidKeySpecException e) {
            Log.e( TAG, "Invalid key specification.");
            throw new IllegalArgumentException(e);
        }
    }

     boolean verifyPurchase(String signedData, String signature) {
        try {
            if (TextUtils.isEmpty(signedData) || TextUtils.isEmpty(m_public_key) ||
                TextUtils.isEmpty(signature)) {
                Log.e( TAG, "Purchase verification failed: missing data.");
                return false;
            }

            PublicKey key = generatePublicKey(m_public_key);
            return verify(key, signedData, signature);
        } catch (Exception e) {
            Log.e( TAG, "verifyPurchase " + e.toString());
            return false;
        }
        
    }
}
