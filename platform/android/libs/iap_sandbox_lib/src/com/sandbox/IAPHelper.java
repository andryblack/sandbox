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

import com.android.vending.billing.IInAppBillingService;

import android.text.TextUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

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

public class IAPHelper  {

    public class IabResult {
        int mResponse;
        String mMessage;

        public IabResult(int response, String message) {
            mResponse = response;
            if (message == null || message.trim().length() == 0) {
                mMessage = getResponseDesc(response);
            }
            else {
                mMessage = message + " (response: " + getResponseDesc(response) + ")";
            }
        }
        public int getResponse() { return mResponse; }
        public String getMessage() { return mMessage; }
        public boolean isSuccess() { return mResponse == BILLING_RESPONSE_RESULT_OK; }
        public boolean isFailure() { return !isSuccess(); }
        public String toString() { return "IabResult: " + getMessage(); }
        public JSONObject toJson() throws JSONException {
            JSONObject obj = new JSONObject();
            obj.put("code",mResponse);
            obj.put("message",mMessage);
            obj.put("success",isSuccess());
            return obj;
        }
    }

    /**
     * Represents an in-app billing product
     */
    class Product {
        String mSku;
        String mPrice;
        String mName;
        String mDescription;
        String mCurrencyCode;
        double mPriceValue;
        public Product( String jsonProduct ) throws JSONException {
            JSONObject o = new JSONObject(jsonProduct);
            mSku = o.optString("productId");
            mPrice = o.optString("price");
            mName = o.optString("title");
            mDescription = o.optString("description");
            mCurrencyCode = o.optString("price_currency_code");
            mPriceValue = o.optLong("price_amount_micros") / 1000000.0;
        }
        public String getSku() { return mSku; }
        public JSONObject toJson() throws JSONException {
            JSONObject obj = new JSONObject();
            obj.put("product_id",mSku);
            obj.put("localized_price",mPrice);
            obj.put("localized_name",mName);
            obj.put("localized_descr",mDescription);
            obj.put("currency_code",mCurrencyCode);
            obj.put("price",mPriceValue);
            return obj;
        }
    }

    /**
     * Represents an in-app billing purchase.
     */
    public class Purchase {
        String mOrderId;
        String mPackageName;
        String mSku;
        long mPurchaseTime;
        int mPurchaseState;
        String mDeveloperPayload;
        String mToken;
        String mOriginalJson;
        String mSignature;
        boolean mIsAutoRenewing;

        public Purchase( String jsonPurchaseInfo, String signature) throws JSONException {
            mOriginalJson = jsonPurchaseInfo;
            JSONObject o = new JSONObject(mOriginalJson);
            mOrderId = o.optString("orderId");
            mPackageName = o.optString("packageName");
            mSku = o.optString("productId");
            mPurchaseTime = o.optLong("purchaseTime");
            mPurchaseState = o.optInt("purchaseState");
            mDeveloperPayload = o.optString("developerPayload");
            mToken = o.optString("token", o.optString("purchaseToken"));
            mSignature = signature;
        }

        public String getOrderId() { return mOrderId; }
        public String getSku() { return mSku; }
        public int getPurchaseState() { return mPurchaseState; }
        public String getDeveloperPayload() { return mDeveloperPayload; }
        public String getToken() { return mToken; }
        public String getOriginalJson() { return mOriginalJson; }
        public String getSignature() { return mSignature; }

        public String getTransaction() throws JSONException {
            JSONObject obj = new JSONObject();
            obj.put("ticket",getOriginalJson());
            obj.put("signature",getSignature());
            return obj.toString();
        }

        public boolean transactionEquals(JSONObject transaction) {
            String ticket = transaction.optString("ticket","");
            String signature = transaction.optString("signature","");
            return ticket.equals(getOriginalJson()) && signature.equals(getSignature());
        }

        @Override
        public String toString() { return "PurchaseInfo:" + mOriginalJson; }
    }

    /**
     * Exception thrown when something went wrong with in-app billing.
     * An IabException has an associated IabResult (an error).
     * To get the IAB result that caused this exception to be thrown,
     * call {@link #getResult()}.
     */
    public class IabException extends Exception {
        IabResult mResult;

        public IabException(IabResult r) {
            this(r, null);
        }
        public IabException(int response, String message) {
            this(new IabResult(response, message));
        }
        public IabException(IabResult r, Exception cause) {
            super(r.getMessage(), cause);
            mResult = r;
        }
        public IabException(int response, String message, Exception cause) {
            this(new IabResult(response, message), cause);
        }

        /** Returns the IAB result (error) that this exception signals. */
        public IabResult getResult() { return mResult; }
    }

   

    private Activity m_activity;
    private String m_public_key;
    private long m_native_object;
    boolean m_setup_done = false;
    // Has this object been disposed of? (If so, we should ignore callbacks, etc)
    boolean m_disposed = false;

    // Is debug logging enabled?
    boolean m_debug_log = true;
    String m_debug_tag = "IAPHelper";

    // Connection to the service
    IInAppBillingService m_service;
    ServiceConnection m_service_conn;

    Map<String,Product> m_products_map = new HashMap<String,Product>();
    Map<String,Purchase> m_purchase_map = new HashMap<String,Purchase>();
   
    // Billing response codes
    public static final int BILLING_RESPONSE_RESULT_OK = 0;
    public static final int BILLING_RESPONSE_RESULT_USER_CANCELED = 1;
    public static final int BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE = 2;
    public static final int BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE = 3;
    public static final int BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE = 4;
    public static final int BILLING_RESPONSE_RESULT_DEVELOPER_ERROR = 5;
    public static final int BILLING_RESPONSE_RESULT_ERROR = 6;
    public static final int BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED = 7;
    public static final int BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED = 8;

    // IAB Helper error codes
    public static final int IABHELPER_ERROR_BASE = -1000;
    public static final int IABHELPER_REMOTE_EXCEPTION = -1001;
    public static final int IABHELPER_BAD_RESPONSE = -1002;
    public static final int IABHELPER_VERIFICATION_FAILED = -1003;
    public static final int IABHELPER_SEND_INTENT_FAILED = -1004;
    public static final int IABHELPER_USER_CANCELLED = -1005;
    public static final int IABHELPER_UNKNOWN_PURCHASE_RESPONSE = -1006;
    public static final int IABHELPER_MISSING_TOKEN = -1007;
    public static final int IABHELPER_UNKNOWN_ERROR = -1008;
    public static final int IABHELPER_SUBSCRIPTIONS_NOT_AVAILABLE = -1009;
    public static final int IABHELPER_INVALID_CONSUMPTION = -1010;
    public static final int IABHELPER_SUBSCRIPTION_UPDATE_NOT_AVAILABLE = -1011;

    // Keys for the responses from InAppBillingService
    public static final String RESPONSE_CODE = "RESPONSE_CODE";
    public static final String RESPONSE_GET_SKU_DETAILS_LIST = "DETAILS_LIST";
    public static final String RESPONSE_BUY_INTENT = "BUY_INTENT";
    public static final String RESPONSE_INAPP_PURCHASE_DATA = "INAPP_PURCHASE_DATA";
    public static final String RESPONSE_INAPP_SIGNATURE = "INAPP_DATA_SIGNATURE";
    public static final String RESPONSE_INAPP_ITEM_LIST = "INAPP_PURCHASE_ITEM_LIST";
    public static final String RESPONSE_INAPP_PURCHASE_DATA_LIST = "INAPP_PURCHASE_DATA_LIST";
    public static final String RESPONSE_INAPP_SIGNATURE_LIST = "INAPP_DATA_SIGNATURE_LIST";
    public static final String INAPP_CONTINUATION_TOKEN = "INAPP_CONTINUATION_TOKEN";

    // Item types
    public static final String ITEM_TYPE_INAPP = "inapp";
    public static final String ITEM_TYPE_SUBS = "subs";

    // some fields on the getSkuDetails response bundle
    public static final String GET_SKU_DETAILS_ITEM_LIST = "ITEM_ID_LIST";
    public static final String GET_SKU_DETAILS_ITEM_TYPE_LIST = "ITEM_TYPE_LIST";

    public  static  final  int PURCHASE_FLOW_RC = 10002;




    IAPOperation m_current_operation = null;
    List<IAPOperation> m_wait_operations = new ArrayList<IAPOperation>();


    class IAPOperation implements Runnable {
        private String m_name;
        public String name() { return m_name; }
        IAPOperation(String name) {
            m_name = name;
        }
        public void Execute() {
            OnStart();
            run();
            OnComplete();
        }
        public void OnStart() {
            IAPHelper.this.OnStartOperation(this);
        }
        public void OnComplete() {
            IAPHelper.this.OnCompleteOperation(this);
        }
        public boolean handleActivityResult(int requestCode, int resultCode, Intent data) {
            return false;
        }
        @Override
        public void run() {

        }
    };

    class IAPAsyncOperation  extends IAPOperation {
        IAPAsyncOperation(String name) {
            super(name);
        }
        @Override
        public void Execute() {
            OnStart();
            final IAPAsyncOperation self = this;
            final Handler handler = new Handler();
            new Thread(new Runnable() {
                @Override
                public void run() {
                    self.run();
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                            self.OnComplete();
                        }
                    });
                }
            }).start();
        }
    };

    public  IAPHelper(Activity activity,long obj) {
        m_activity = activity;
        m_native_object = obj;
    }

    void OnStartOperation(IAPOperation op) {
        logInfo("Start operation " + op.name());  
        m_current_operation = op;
    }

    void OnCompleteOperation(IAPOperation op) {
        logInfo("Complete operation " + op.name());  
        if (m_current_operation != op) {
            logError("invalid operation: " + (m_current_operation==null ? "null" : m_current_operation.name()));
        }
        if (m_disposed) {
            return;
        }
        m_current_operation = null;
        if (!m_wait_operations.isEmpty()) {
            logDebug("Starting waited thread operation");
            IAPOperation nextop = m_wait_operations.remove(0);
            nextop.Execute();
        }
    }
    void StartOperation(IAPOperation op) {
        
        if (m_current_operation == null) {
            op.Execute();
        } else {
            logInfo("Schedule operation " + op.name() + " becouse " + m_current_operation.name() + " active");  
            m_wait_operations.add(op);
        }
    }

    private boolean start_setup(String signature) {
        
        m_public_key = signature;
        if (m_disposed)
            return false;

        if (m_setup_done) return false;

        m_service_conn = new ServiceConnection() {
            @Override
            public void onServiceDisconnected(ComponentName name) {
                logDebug("Billing service disconnected.");
                m_service = null;
                m_setup_done = false;
            }

            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                if (m_disposed) return;
                logDebug("Billing service connected.");
                m_service = IInAppBillingService.Stub.asInterface(service);
                String packageName = m_activity.getPackageName();
                try {
                    logDebug("Checking for in-app billing 3 support.");

                    // check for in-app billing v3 support
                    int response = m_service.isBillingSupported(3, packageName, ITEM_TYPE_INAPP);
                    if (response != BILLING_RESPONSE_RESULT_OK) {
                        on_iap_init_finished(new IabResult(response,
                                "Error checking for billing v3 support."));

                        return;
                    } else {
                        logDebug("In-app billing version 3 supported for " + packageName);
                    }

                    // Check for v5 subscriptions support. This is needed for
                    // getBuyIntentToReplaceSku which allows for subscription update
                    response = m_service.isBillingSupported(5, packageName, ITEM_TYPE_SUBS);
                    if (response == BILLING_RESPONSE_RESULT_OK) {
                        logDebug("Subscription re-signup AVAILABLE.");
                    } else {
                        logDebug("Subscription re-signup not available.");
                    }


                    m_setup_done = true;
                }
                catch (RemoteException e) {
                    on_iap_init_finished(new IabResult(IABHELPER_REMOTE_EXCEPTION,
                                "RemoteException while setting up in-app billing."));
                    e.printStackTrace();
                    return;
                }

                on_iap_init_finished(new IabResult(BILLING_RESPONSE_RESULT_OK, "Setup successful."));
            }
        };

        Intent serviceIntent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
        serviceIntent.setPackage("com.android.vending");
        List<ResolveInfo> intentServices = m_activity.getPackageManager().queryIntentServices(serviceIntent, 0);
        if (intentServices != null && !intentServices.isEmpty()) {
            // service available to handle that Intent
            m_activity.bindService(serviceIntent, m_service_conn, Context.BIND_AUTO_CREATE);
        }
        else {
            // no service available to handle that Intent
            on_iap_init_finished(
                    new IabResult(BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE,
                            "Billing service unavailable on device."));
        }
        return true;
    
    }

    public boolean iap_init(String key) {
        boolean res = false;
        if (m_setup_done)
            return true;
        try {
            res = start_setup(key);
        } catch (Exception e) {
            logError("iap_init: " + e.getMessage());
        }
        return res;
    }


    /**
     * Dispose of object, releasing resources. It's very important to call this
     * method when you are done with this object. It will release any resources
     * used by it such as service connections. Naturally, once the object is
     * disposed of, it can't be used again.
     */
    public void dispose()  {
        logDebug("Disposing.");
        m_setup_done = false;
        try {
            if (m_service_conn != null) {
                logDebug("Unbinding from service.");
                if (m_activity != null) 
                    m_activity.unbindService(m_service_conn);
            }
        } catch (Exception e) {
            logError("dispose: " + e.getMessage());
        }
        m_disposed = true;
        m_activity = null;
        m_service_conn = null;
        m_service = null;
        m_native_object = 0;
    }


    class QueryPurchases  extends IAPAsyncOperation {

        public QueryPurchases() {
            super("QueryPurchases");
        }

        Map<String,Purchase> m_purchase_map = new HashMap<String,Purchase>();

        private int asyncQueryPurchases() throws JSONException, RemoteException {
            // Query purchases
            logDebug("Querying owned items");
            logDebug("Package name: " + m_activity.getPackageName());
            boolean verificationFailed = false;
            String continueToken = null;

            do {
                logDebug("Calling getPurchases with continuation token: " + continueToken);
                Bundle ownedItems = m_service.getPurchases(3, m_activity.getPackageName(),
                        ITEM_TYPE_INAPP, continueToken);

                int response = getResponseCodeFromBundle(ownedItems);
                logDebug("Owned items response: " + String.valueOf(response));
                if (response != BILLING_RESPONSE_RESULT_OK) {
                    logDebug("getPurchases() failed: " + getResponseDesc(response));
                    return response;
                }
                if (!ownedItems.containsKey(RESPONSE_INAPP_ITEM_LIST)
                        || !ownedItems.containsKey(RESPONSE_INAPP_PURCHASE_DATA_LIST)
                        || !ownedItems.containsKey(RESPONSE_INAPP_SIGNATURE_LIST)) {
                    logError("Bundle returned from getPurchases() doesn't contain required fields.");
                    return IABHELPER_BAD_RESPONSE;
                }

                ArrayList<String> ownedSkus = ownedItems.getStringArrayList(
                        RESPONSE_INAPP_ITEM_LIST);
                ArrayList<String> purchaseDataList = ownedItems.getStringArrayList(
                        RESPONSE_INAPP_PURCHASE_DATA_LIST);
                ArrayList<String> signatureList = ownedItems.getStringArrayList(
                        RESPONSE_INAPP_SIGNATURE_LIST);

                for (int i = 0; i < purchaseDataList.size(); ++i) {
                    String purchaseData = purchaseDataList.get(i);
                    String signature = signatureList.get(i);
                    String sku = ownedSkus.get(i);
                    if (verifyPurchase(m_public_key, purchaseData, signature)) {
                        logDebug("Sku is owned: " + sku);
                        logDebug("   Purchase data: " + purchaseData);
                        logDebug("   Signature: " + signature);
                        Purchase purchase = new Purchase(purchaseData, signature);

                        if (TextUtils.isEmpty(purchase.getToken())) {
                            logWarn("BUG: empty/null token!");
                            logDebug("Purchase data: " + purchaseData);
                        }
                        // Record ownership and token
                        m_purchase_map.put(purchase.getSku(),purchase);
                    }
                    else {
                        logWarn("Purchase signature verification **FAILED**. Not adding item: " + sku );
                        logDebug("   Purchase data: " + purchaseData);
                        logDebug("   Signature: " + signature);
                        verificationFailed = true;
                    }
                }

                continueToken = ownedItems.getString(INAPP_CONTINUATION_TOKEN);
                logDebug("Continuation token: " + continueToken);
            } while (!TextUtils.isEmpty(continueToken));

            return verificationFailed ? IABHELPER_VERIFICATION_FAILED : BILLING_RESPONSE_RESULT_OK;
        }

        @Override
        public void run() {
            try {
                int r = asyncQueryPurchases();
                if (r != BILLING_RESPONSE_RESULT_OK) {
                    throw new IabException(r, "Error restore payments.");
                }
            }
            catch (IabException ex) {
                logError(ex.getMessage());
            }catch (RemoteException ex) {
                logError(ex.getMessage());
            }catch (JSONException ex) {
                logError(ex.getMessage());
            }
        }

        @Override
        public void OnComplete() {
            on_iap_restore_payments_finished(m_purchase_map);
            super.OnComplete();
        }

    };


    class QueryProducts  extends IAPAsyncOperation {
        List<String> m_sku_list;
        IabResult m_result;
        Map<String,Product> m_products_map = new HashMap<String,Product>();

        QueryProducts(List<String> sku_list) {
            super("QueryProducts");
            m_sku_list = sku_list;
            m_result = null;
        }

        private int querySkuDetails(String itemType)
            throws RemoteException {
            logDebug("Querying SKU details.");
            ArrayList<String> skuList = new ArrayList<String>();

            if (m_sku_list != null) {
                for (String sku : m_sku_list) {
                    if (!skuList.contains(sku)) {
                        logDebug("sku:" + sku);
                        skuList.add(sku);
                    }
                }
            }

            if (skuList.size() == 0) {
                logDebug("queryPrices: nothing to do because there are no SKUs.");
                return BILLING_RESPONSE_RESULT_OK;
            }

            // Split the sku list in blocks of no more than 20 elements.
            ArrayList<ArrayList<String>> packs = new ArrayList<ArrayList<String>>();
            ArrayList<String> tempList;
            int n = skuList.size() / 20;
            int mod = skuList.size() % 20;
            for (int i = 0; i < n; i++) {
                tempList = new ArrayList<String>();
                for (String s : skuList.subList(i * 20, i * 20 + 20)) {
                    tempList.add(s);
                }
                packs.add(tempList);
            }
            if (mod != 0) {
                tempList = new ArrayList<String>();
                for (String s : skuList.subList(n * 20, n * 20 + mod)) {
                    tempList.add(s);
                }
                packs.add(tempList);
            }

            for (ArrayList<String> skuPartList : packs) {
                Bundle querySkus = new Bundle();
                logDebug("Request details: " + skuPartList.size());
                querySkus.putStringArrayList(GET_SKU_DETAILS_ITEM_LIST, skuPartList);
                Bundle skuDetails = m_service.getSkuDetails(3, m_activity.getPackageName(),
                        itemType, querySkus);

                if (!skuDetails.containsKey(RESPONSE_GET_SKU_DETAILS_LIST)) {
                    int response = getResponseCodeFromBundle(skuDetails);
                    if (response != BILLING_RESPONSE_RESULT_OK) {
                        logDebug("getSkuDetails() failed: " + getResponseDesc(response));
                        return response;
                    } else {
                        logError("getSkuDetails() returned a bundle with neither an error nor a detail list.");
                        return IABHELPER_BAD_RESPONSE;
                    }
                }

                ArrayList<String> responseList = skuDetails.getStringArrayList(
                        RESPONSE_GET_SKU_DETAILS_LIST);

                logDebug("Got sku details: " + responseList.size());

                for (String thisResponse : responseList) {
                    try {
                        Product product = new Product(thisResponse);
                        m_products_map.put(product.getSku(),product);
                    } catch (JSONException e) {
                        logError("Failed parse product " + e.toString());
                    }
                    logDebug("Got sku details: " + thisResponse);
                }
            }

            return BILLING_RESPONSE_RESULT_OK;
        }

        /**
         * Queries the inventory. This will query all owned items from the server, as well as
         * information on additional skus, if specified. This method may block or take long to execute.
         * Do not call from a UI thread. For that, use the non-blocking version {@link #queryInventoryAsync}.
         *
         * @param moreItemSkus additional PRODUCT skus to query information on, regardless of ownership.
         *     Ignored if null or if querySkuDetails is false.
         * @throws IabException if a problem occurs while refreshing the inventory.
         */
        private void queryInventory() throws IabException {
            try {

                int r = querySkuDetails(ITEM_TYPE_INAPP);
                if (r != BILLING_RESPONSE_RESULT_OK) {
                    throw new IabException(r, "Error refreshing inventory (querying prices of items).");
                }

                return;
            }
            catch (RemoteException e) {
                throw new IabException(IABHELPER_REMOTE_EXCEPTION, "Remote exception while refreshing inventory.", e);
            }
        }


        @Override
        public void run() {
            IabResult result = new IabResult(BILLING_RESPONSE_RESULT_OK, "Inventory refresh successful.");
            try {
                queryInventory();
            }
            catch (IabException ex) {
                result = ex.getResult();
            }
            m_result = result;
        }
        @Override
        public void OnComplete() {
            on_iap_get_products_info_finished(m_products_map,m_result);
            super.OnComplete();
        }
    };
    

    class ConsumePurchase  extends IAPAsyncOperation {
        Purchase m_item;
        IabResult m_result;

        public ConsumePurchase( Purchase p ) {
            super("ConsumePurchase");
            m_item = p;
        }
        /**
         * Consumes a given in-app product. Consuming can only be done on an item
         * that's owned, and as a result of consumption, the user will no longer own it.
         * This method may block or take long to return. Do not call from the UI thread.
         *
         * @param itemInfo The PurchaseInfo that represents the item to consume.
         * @throws IabException if there is a problem during consumption.
         */
        private void consume(Purchase itemInfo) throws IabException {
            check_setup_done("consume");


            try {
                String token = itemInfo.getToken();
                String sku = itemInfo.getSku();
                if (token == null || token.equals("")) {
                    logError("Can't consume "+ sku + ". No token.");
                    throw new IabException(IABHELPER_MISSING_TOKEN, "PurchaseInfo is missing token for sku: "
                            + sku + " " + itemInfo);
                }

                logDebug("Consuming sku: " + sku + ", token: " + token);
                int response = m_service.consumePurchase(3, m_activity.getPackageName(), token);
                if (response == BILLING_RESPONSE_RESULT_OK) {
                    logDebug("Successfully consumed sku: " + sku);
                }
                else {
                    logDebug("Error consuming consuming sku " + sku + ". " + getResponseDesc(response));
                    throw new IabException(response, "Error consuming sku " + sku);
                }
            }
            catch (RemoteException e) {
                throw new IabException(IABHELPER_REMOTE_EXCEPTION, "Remote exception while consuming. PurchaseInfo: " + itemInfo, e);
            }
        }

        @Override
        public void run() {
            IabResult result;
            try {
                consume(m_item);
                result = new IabResult(BILLING_RESPONSE_RESULT_OK, "Successful consume of sku " + m_item.getSku());
            }
            catch (IabException ex) {
                result = ex.getResult();
            }
            m_result = result;
        }

        @Override 
        public void OnComplete() {
            on_iap_confirm_transaction_finished(m_item, m_result);
            super.OnComplete();
        }
        

    };
    

    class PurchaseFlow  extends IAPOperation {
        String m_sku;
        String m_extra_data;
        public PurchaseFlow(String sku,String extraData) {
            super("PurchaseFlow");
            m_sku = sku;
            m_extra_data = extraData;
        }
        @Override
        public void OnComplete() {

        }
        @Override
        public void run() {
            IabResult result;


            try {
                logDebug("Constructing buy intent for " + m_sku );
               
                // Purchasing a new item
                Bundle buyIntentBundle = m_service.getBuyIntent(3, m_activity.getPackageName(), m_sku, ITEM_TYPE_INAPP,
                        m_extra_data);
                int response = getResponseCodeFromBundle(buyIntentBundle);
                if (response != BILLING_RESPONSE_RESULT_OK) {
                    logError("Unable to buy item, Error response: " + getResponseDesc(response));
                    result = new IabResult(response, "Unable to buy item");
                    on_iap_purchase_finished(m_sku, result, null);
                    return;
                }

                PendingIntent pendingIntent = buyIntentBundle.getParcelable(RESPONSE_BUY_INTENT);
                logDebug("Launching buy intent for " + m_sku );
                m_activity.startIntentSenderForResult(pendingIntent.getIntentSender(),
                        PURCHASE_FLOW_RC, new Intent(),
                        Integer.valueOf(0), Integer.valueOf(0),
                        Integer.valueOf(0));
            }
            catch (SendIntentException e) {
                logError("SendIntentException while launching purchase flow for sku " + m_sku);
                e.printStackTrace();
                
                result = new IabResult(IABHELPER_SEND_INTENT_FAILED, "Failed to send intent.");
                on_iap_purchase_finished(m_sku, result,  null);
            }
            catch (RemoteException e) {
                logError("RemoteException while launching purchase flow for sku " + m_sku);
                e.printStackTrace();
                
                result = new IabResult(IABHELPER_REMOTE_EXCEPTION, "Remote exception while starting purchase flow");
                on_iap_purchase_finished(m_sku, result, null);
            }
        } 

        @Override
        public boolean handleActivityResult(int requestCode, int resultCode, Intent data) {
            IabResult result;
            
            try {

                if (data == null) {
                    logError("Null data in IAB activity result.");
                    result = new IabResult(IABHELPER_BAD_RESPONSE, "Null data in IAB result");
                    on_iap_purchase_finished(m_sku, result,   null);
                    return true;
                }

                int responseCode = getResponseCodeFromIntent(data);
                String purchaseData = data.getStringExtra(RESPONSE_INAPP_PURCHASE_DATA);
                String dataSignature = data.getStringExtra(RESPONSE_INAPP_SIGNATURE);

                if (resultCode == Activity.RESULT_OK && responseCode == BILLING_RESPONSE_RESULT_OK) {
                    logDebug("Successful resultcode from purchase activity.");
                    logDebug("Purchase data: " + purchaseData);
                    logDebug("Data signature: " + dataSignature);
                    logDebug("Extras: " + data.getExtras());
                    //logDebug("Expected item type: " + mPurchasingItemType);

                    if (purchaseData == null || dataSignature == null) {
                        logError("BUG: either purchaseData or dataSignature is null.");
                        logDebug("Extras: " + data.getExtras().toString());
                        result = new IabResult(IABHELPER_UNKNOWN_ERROR, "IAB returned null purchaseData or dataSignature");
                        on_iap_purchase_finished(m_sku, result,  null);
                        return true;
                    }

                    Purchase purchase = null;
                    try {
                        purchase = new Purchase( purchaseData, dataSignature);
                        String sku = purchase.getSku();

                        // Verify signature
                        if (!verifyPurchase(m_public_key, purchaseData, dataSignature)) {
                            logError("Purchase signature verification FAILED for sku " + sku);
                            result = new IabResult(IABHELPER_VERIFICATION_FAILED, "Signature verification failed for sku " + sku);
                            on_iap_purchase_finished(m_sku, result, purchase);
                            return true;
                        }
                        logDebug("Purchase signature successfully verified.");
                    }
                    catch (JSONException e) {
                        logError("Failed to parse purchase data.");
                        e.printStackTrace();
                        result = new IabResult(IABHELPER_BAD_RESPONSE, "Failed to parse purchase data.");
                        on_iap_purchase_finished(m_sku, result, null);
                        return true;
                    }

                    on_iap_purchase_finished(m_sku,new IabResult(BILLING_RESPONSE_RESULT_OK, "Success"), purchase);
                }
                else if (resultCode == Activity.RESULT_OK) {
                    // result code was OK, but in-app billing response was not OK.
                    logDebug("Result code was OK but in-app billing response was not OK: " + getResponseDesc(responseCode));
                    result = new IabResult(responseCode, "Problem purchashing item:" + getResponseDesc(responseCode));
                    on_iap_purchase_finished(m_sku,result, null);
                }
                else if (resultCode == Activity.RESULT_CANCELED) {
                    logDebug("Purchase canceled - Response: " + getResponseDesc(responseCode));
                    if (responseCode != BILLING_RESPONSE_RESULT_OK) {
                        result = new IabResult(responseCode, "cancled");
                    } else {
                        result = new IabResult(IABHELPER_USER_CANCELLED, "cancled/ok");
                    } 
                    on_iap_purchase_finished(m_sku,result, null);
                }
                else {
                    logError("Purchase failed. Result code: " + Integer.toString(resultCode)
                            + ". Response: " + getResponseDesc(responseCode));
                    result = new IabResult(IABHELPER_UNKNOWN_PURCHASE_RESPONSE, "Unknown purchase response: " + getResponseDesc(responseCode));
                    on_iap_purchase_finished(m_sku,result, null);
                }
                return true;
            } catch (Exception e) {
                logError("handleActivityResult: " + e.toString());
            }
            return false;
        }

    };


    /**
     * Handles an activity result that's part of the purchase flow in in-app billing. If you
     * are calling {@link #launchPurchaseFlow}, then you must call this method from your
     * Activity's {@link android.app.Activity@onActivityResult} method. This method
     * MUST be called from the UI thread of the Activity.
     *
     * @param requestCode The requestCode as you received it.
     * @param resultCode The resultCode as you received it.
     * @param data The data (Intent) as you received it.
     * @return Returns true if the result was related to a purchase flow and was handled;
     *     false if the result was not related to a purchase, in which case you should
     *     handle it normally.
     */
    public boolean handleActivityResult(int requestCode, int resultCode, Intent data) {
        IabResult result;
        if (requestCode != PURCHASE_FLOW_RC) return false;
        if (m_current_operation == null) return false;
        if (m_current_operation.handleActivityResult(requestCode,resultCode,data)) {
            OnCompleteOperation(m_current_operation);
            return true;
        }
        return false;
    }

    

    public boolean is_setup_done() {
        return m_setup_done;
    }

    public void on_query_inventory(ArrayList<String> products) {
        StartOperation(new QueryProducts(products));
    }

    public boolean iap_get_products_info(String json_list) {
        try {
            JSONArray jsonArray = new JSONArray(json_list);
            ArrayList<String> stringArray = new ArrayList<String>();
            for(int i = 0, count = jsonArray.length(); i< count; i++)
            {
                stringArray.add(jsonArray.getString(i));
            }
            on_query_inventory(stringArray);
        } catch ( JSONException e ) {
            logError(e.toString());
            return false;
        } catch ( Exception e ) {
            logError(e.toString());
            return false;
        }
        return true;
    }

    public boolean iap_restore_payments() {
        if (m_disposed) return false;
        try {
            StartOperation(new QueryPurchases());
        } catch (Exception e) {
            logError(e.toString());
            return false;
        }
        return true;
    }

    public boolean iap_purchase(String product,String extraData) {
        try {
            StartOperation(new PurchaseFlow(product,extraData));
        } catch ( Exception e ) {
            logError(e.toString());
            return false;

        }
        return true;
    }

    public void on_consume_purchase(Purchase purchase,JSONObject transaction)  {
        StartOperation(new ConsumePurchase(purchase));
    }

    public boolean iap_confirm_transaction(String signature) {
        logDebug("iap_confirm_transaction :" + signature);
        boolean result = false;
        try {
            JSONObject transaction = new JSONObject(signature);
            for (Purchase purchase: m_purchase_map.values()) {
                try {
                    logDebug("check purchase :" + purchase.getSku());
                    if (purchase.transactionEquals(transaction)) {
                        logDebug("found purchase for confirm " + purchase.getSku());
                        on_consume_purchase(purchase,transaction);
                        result = true;
                    }
                } catch ( JSONException e) {
                    logError(e.toString());
                }
            }
        } catch ( Exception e ) {
            logError(e.toString());
            return false;

        }
        return result;
    }

    private static native void nativeProcessResponse(long obj,String method,String data);

    private JSONArray buildProductsJson() throws JSONException {
        JSONArray a = new JSONArray();
        for(Product product:m_products_map.values())
        {
            a.put(product.toJson());
        }
        return a;
    }

    /// listeners
    void on_iap_restore_payments_finished(Map<String,Purchase> purchase_map) {
        for (Purchase purchase: purchase_map.values()) {
            m_purchase_map.put(purchase.getSku(),purchase);
        }
        for (Purchase purchase: m_purchase_map.values()) {
            try {
                JSONObject obj = new JSONObject();
                obj.put("product_id", purchase.getSku());
                obj.put("state", "restored");
                obj.put("transaction", purchase.getTransaction());
                nativeProcessResponse(m_native_object, "iap_restore_payments", obj.toString());
            } catch ( JSONException e) {
                logError(e.toString());
            }
        }
    }
    void on_iap_get_products_info_finished(Map<String,Product> products,IabResult result) {
        for (Product p: products.values()) {
            m_products_map.put(p.getSku(),p);
        }
        JSONObject obj = new JSONObject();
        try {
            obj.put("result", result.toJson());
        } catch (JSONException e) {
           logError(e.toString());
        }
        if (result.isSuccess()) {
            try {
                obj.put("products", buildProductsJson());
            } catch (JSONException e) {
                logError(e.toString());
            }
        }
        nativeProcessResponse(m_native_object,"iap_get_products_info",obj.toString());
    }

    void on_iap_init_finished(IabResult result) {
        try {
            nativeProcessResponse(m_native_object,"iap_init",result.toJson().toString());
        } catch (JSONException e) {
            logError(e.toString());
        }
    }


    void on_iap_purchase_finished(String sku,IabResult result, Purchase info) {
        try {
            JSONObject obj = new JSONObject();
            obj.put("product_id",sku);
            obj.put("result",result.toJson());
            obj.put("state",result.isSuccess() ? "purchased" : "failed");
            if (info != null) {
                obj.put("transaction",info.getTransaction());
                m_purchase_map.put(info.getSku(),info);
            }
            nativeProcessResponse(m_native_object,"iap_purchase",obj.toString());
        } catch (JSONException e) {
            logError(e.toString());
        }
    }

    void on_iap_confirm_transaction_finished(Purchase purchase, IabResult result) {
        try {
            if (result.isSuccess()) {
                m_purchase_map.remove(purchase.getSku());
            }

            JSONObject obj = new JSONObject();
            obj.put("product_id",purchase.getSku());
            obj.put("result",result.toJson());
            obj.put("state",result.isSuccess() ? "success" : "failed");
            nativeProcessResponse(m_native_object,"iap_confirm_transaction",obj.toString());
        } catch (JSONException e) {
            logError(e.toString());
        }
    }

    // Workaround to bug where sometimes response codes come as Long instead of Integer
    private int getResponseCodeFromBundle(Bundle b) {
        Object o = b.get(RESPONSE_CODE);
        if (o == null) {
            logDebug("Bundle with null response code, assuming OK (known issue)");
            return BILLING_RESPONSE_RESULT_OK;
        }
        else if (o instanceof Integer) return ((Integer)o).intValue();
        else if (o instanceof Long) return (int)((Long)o).longValue();
        else {
            logError("Unexpected type for bundle response code.");
            logError(o.getClass().getName());
            throw new RuntimeException("Unexpected type for bundle response code: " + o.getClass().getName());
        }
    }

    /**
     * Returns a human-readable description for the given response code.
     *
     * @param code The response code
     * @return A human-readable string explaining the result code.
     *     It also includes the result code numerically.
     */
    private static String getResponseDesc(int code) {
        String[] iab_msgs = ("0:OK/1:User Canceled/2:Unknown/" +
                "3:Billing Unavailable/4:Item unavailable/" +
                "5:Developer Error/6:Error/7:Item Already Owned/" +
                "8:Item not owned").split("/");
        String[] iabhelper_msgs = ("0:OK/-1001:Remote exception during initialization/" +
                "-1002:Bad response received/" +
                "-1003:Purchase signature verification failed/" +
                "-1004:Send intent failed/" +
                "-1005:User cancelled/" +
                "-1006:Unknown purchase response/" +
                "-1007:Missing token/" +
                "-1008:Unknown error/" +
                "-1009:Subscriptions not available/" +
                "-1010:Invalid consumption attempt").split("/");

        if (code <= IABHELPER_ERROR_BASE) {
            int index = IABHELPER_ERROR_BASE - code;
            if (index >= 0 && index < iabhelper_msgs.length) return iabhelper_msgs[index];
            else return String.valueOf(code) + ":Unknown IAB Helper Error";
        }
        else if (code < 0 || code >= iab_msgs.length)
            return String.valueOf(code) + ":Unknown";
        else
            return iab_msgs[code];
    }

    // Workaround to bug where sometimes response codes come as Long instead of Integer
    private int getResponseCodeFromIntent(Intent i) {
        Object o = i.getExtras().get(RESPONSE_CODE);
        if (o == null) {
            logError("Intent with no response code, assuming OK (known issue)");
            return BILLING_RESPONSE_RESULT_OK;
        }
        else if (o instanceof Integer) return ((Integer)o).intValue();
        else if (o instanceof Long) return (int)((Long)o).longValue();
        else {
            logError("Unexpected type for intent response code.");
            logError(o.getClass().getName());
            throw new RuntimeException("Unexpected type for intent response code: " + o.getClass().getName());
        }
    }


    // Checks that setup was done; if not, throws an exception.
    void check_setup_done(String operation) throws IllegalStateException {
        if (!m_setup_done) {
            logError("Illegal state for operation (" + operation + "): IAB helper is not set up.");
            throw new IllegalStateException("IAB helper is not set up. Can't perform operation: " + operation);
        }
    }

    void logDebug(String msg) {
        if (m_debug_log) Log.d(m_debug_tag, msg);
    }

    void logError(String msg) {
        Log.e(m_debug_tag, msg);
    }

    void logWarn(String msg) {
        Log.w(m_debug_tag, msg);
    }

    void logInfo(String msg) {
        Log.i(m_debug_tag, msg);
    }


    private static final String KEY_FACTORY_ALGORITHM = "RSA";
    private static final String SIGNATURE_ALGORITHM = "SHA1withRSA";

    boolean verify(PublicKey publicKey, String signedData, String signature) {
        byte[] signatureBytes;
        try {
            signatureBytes = Base64.decode(signature, Base64.DEFAULT);
        } catch (IllegalArgumentException e) {
            logError( "Base64 decoding failed.");
            return false;
        }
        try {
            Signature sig = Signature.getInstance(SIGNATURE_ALGORITHM);
            sig.initVerify(publicKey);
            sig.update(signedData.getBytes());
            if (!sig.verify(signatureBytes)) {
                logError( "Signature verification failed.");
                return false;
            }
            return true;
        } catch (NoSuchAlgorithmException e) {
            logError( "NoSuchAlgorithmException.");
        } catch (InvalidKeyException e) {
            logError( "Invalid key specification.");
        } catch (SignatureException e) {
            logError( "Signature exception.");
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
            logError( "Invalid key specification.");
            throw new IllegalArgumentException(e);
        }
    }

     boolean verifyPurchase(String base64PublicKey, String signedData, String signature) {
        try {
            if (TextUtils.isEmpty(signedData) || TextUtils.isEmpty(base64PublicKey) ||
                TextUtils.isEmpty(signature)) {
                logError( "Purchase verification failed: missing data.");
                return false;
            }

            PublicKey key = generatePublicKey(base64PublicKey);
            return verify(key, signedData, signature);
        } catch (Exception e) {
            logError("verifyPurchase " + e.toString());
            return false;
        }
        
    }
}
