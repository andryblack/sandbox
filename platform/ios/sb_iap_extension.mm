#include "sb_iap_extension.h"

#include <sbstd/sb_intrusive_ptr.h>
#include "sb_application.h"
#include "sb_ios_extension.h"
#include "json/sb_json.h"
#include "utils/sb_base64.h"

static const char* MODULE = "iap";

@interface iap_manager() {
    Sandbox::Application* m_application;
    NSMutableDictionary<NSString*,SKProduct*> *m_products;
}
@end

@implementation iap_manager

-(id)init {
    if (self = [super init]) {
        m_products = [[NSMutableDictionary alloc] init];
        m_application = 0;
    }
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    return self;
}
-(void)dealloc {
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    [super dealloc];
    [m_products release];
}

-(SKProduct*)getProductWithIdentifier:(NSString*)identifier {
    return [m_products objectForKey:identifier];
}

-(void)setApplication:(Sandbox::Application*) app {
    m_application = app;
}

-(NSString*) reqiestProductsInformation:(NSArray*)products{
    if (!products || products.count == 0)
        return nil;
    SKProductsRequest *productsRequest = [[SKProductsRequest alloc]
                                          initWithProductIdentifiers:[NSSet setWithArray:products]];
    productsRequest.delegate = self;
    [productsRequest start];
    return [NSString stringWithFormat:@"%p",productsRequest];
}
-(void) doPaymentResponse:(SKPaymentTransaction*) transaction {
    
    NSData* json_encoded = [NSJSONSerialization dataWithJSONObject:[NSDictionary dictionaryWithObjectsAndKeys:
                                                                    transaction.payment.productIdentifier,@"product_id",
                                                                    @"purchased",@"state",
                                                                    [self getReceiptForTransaction:transaction],@"transaction",
                                                                    nil] options:0 error:nil];
    
    NSString* newStr = [[NSString alloc] initWithData:json_encoded encoding:NSUTF8StringEncoding];
    if (m_application)
        m_application->OnExtensionResponse("iap_restore_payments",newStr.UTF8String);
    [newStr release];
}

-(NSString*) performPayment:(NSString*) productIdentifier status:(NSString**) status {
    for (SKPaymentTransaction *transaction in [[SKPaymentQueue defaultQueue] transactions]) {
        if ([transaction.payment.productIdentifier isEqualToString:productIdentifier]) {
            if (transaction.transactionState == SKPaymentTransactionStatePurchased) {
                SB_LOGE("already purchased: " << productIdentifier.UTF8String);
                if (status) {
                    *status = @"already_purchased";
                }
                return nil;
            }
        }
    }
    SKProduct* product = [m_products objectForKey:productIdentifier];
    if (!product) {
        if (status) {
            *status = @"unknown_product";
        }
        return nil;
    }
    SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:product];
    payment.quantity = 1;
    [[SKPaymentQueue defaultQueue] addPayment:payment];
    return [NSString stringWithFormat:@"%p",payment];
}
-(NSString*) refreshReceipt {
    SKReceiptRefreshRequest* request = [[SKReceiptRefreshRequest alloc] init];
    request.delegate = self;
    [request performSelector:@selector(start) withObject:nil afterDelay:0.01];
    return [NSString stringWithFormat:@"%p",request];
}
-(void) restorePayments {
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    
    for (SKPaymentTransaction *transaction in [[SKPaymentQueue defaultQueue] transactions]) {
        if (transaction.transactionState == SKPaymentTransactionStatePurchased) {
            [self performSelector:@selector(doPaymentResponse:) withObject:transaction afterDelay:0.01];
        }
    }
}

-(void)finishTransaction:(SKPaymentTransaction*) transaction {
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(BOOL)confirmTransaction:(NSString*)transactionData {
    sb::string transaction_id;
    sb::map<sb::string, sb::string> transaction_data;
    if (Sandbox::json_parse_object(transactionData.UTF8String, transaction_data)) {
        transaction_id = transaction_data["id"];
    } else {
        transaction_id = transactionData.UTF8String;
    }
    if (transaction_id.empty())
        return FALSE;
    NSString* transactionIdentifier = [NSString stringWithUTF8String:transaction_id.c_str()];
    for (SKPaymentTransaction *transaction in [[SKPaymentQueue defaultQueue] transactions]) {
        if (transaction.transactionState == SKPaymentTransactionStatePurchased ||
            transaction.transactionState == SKPaymentTransactionStateRestored) {
            if ([transaction.transactionIdentifier isEqual:transactionIdentifier]) {
                [self finishTransaction:transaction];
                return TRUE;
            }
        }
    }
    return FALSE;
}

// SKRequestDelegate
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    SB_LOGE("request: " << request.description.UTF8String << " didFailWithError: " << error.description.UTF8String);
    if (m_application) {
        NSData* json_encoded = [NSJSONSerialization dataWithJSONObject:[NSDictionary dictionaryWithObjectsAndKeys:
                                                                        [NSString stringWithFormat:@"%p",request],@"request_id",
                                                                        error.description,@"error", nil] options:0 error:nil];
        NSString* newStr = [[NSString alloc] initWithData:json_encoded encoding:NSUTF8StringEncoding];
        m_application->OnExtensionResponse("iap_get_products_info",newStr.UTF8String);
        [newStr release];
    }
}


// SKProductsRequestDelegate protocol method
- (void)productsRequest:(SKProductsRequest *)request
     didReceiveResponse:(SKProductsResponse *)response
{
    NSMutableArray* products_info = [NSMutableArray arrayWithCapacity:response.products.count];
    for (SKProduct* product in response.products) {
        NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFormatter setLocale:product.priceLocale];
        NSString *formattedString = [numberFormatter stringFromNumber:product.price];
        NSString *currencyCode = numberFormatter.currencyCode;
        [numberFormatter release];
        
        
        [products_info addObject:[NSDictionary dictionaryWithObjectsAndKeys:
                                  product.productIdentifier,
                                  @"product_id",
                                  product.price,
                                  @"price",
                                  formattedString,
                                  @"localized_price",
                                  currencyCode,
                                  @"currency_code",
                                  nil]];
        [m_products setObject:product forKey:product.productIdentifier];
    }
    NSData* json_encoded = [NSJSONSerialization dataWithJSONObject:[NSDictionary dictionaryWithObjectsAndKeys:
                                                                      products_info,@"products",
                                                                      response.invalidProductIdentifiers,@"invalid",
                                                                    [NSString stringWithFormat:@"%p",request],@"request_id",
                                                                    nil] options:0 error:nil];
    if (m_application) {
        NSString* newStr = [[NSString alloc] initWithData:json_encoded encoding:NSUTF8StringEncoding];
        m_application->OnExtensionResponse("iap_get_products_info",newStr.UTF8String);
        [newStr release];
    }
    [request release];
}

- (NSString*) encodeTransaction:(SKPaymentTransaction*) transaction receipt:(NSData*) receipt{
    NSDictionary* dict = [NSDictionary dictionaryWithObjectsAndKeys:
     transaction.transactionIdentifier,@"id",
     [receipt base64EncodedStringWithOptions:0],@"receipt",
                          nil];
    NSData* json_encoded = [NSJSONSerialization dataWithJSONObject:dict options:0 error:nil];
    return [[[NSString alloc] initWithData:json_encoded encoding:NSUTF8StringEncoding] autorelease];
}

- (NSString*) getReceiptForTransaction:(SKPaymentTransaction*) transaction {
    // Load the receipt from the app bundle.
    NSBundle* b = [NSBundle mainBundle];
    if ([b respondsToSelector:@selector(appStoreReceiptURL)]) {
        NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
        if (!receiptURL) {
            return @"error:nourl";
        }
        NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
        if (!receipt) {
            return @"error:nodata";
        }
        if (receipt) { /* No local receipt -- handle the error. */
            return [self encodeTransaction:transaction receipt:receipt];
        }
    } else {
        NSData* receipt = transaction.transactionReceipt;
        if (receipt) {
            return [self encodeTransaction:transaction receipt:receipt];
        }
    }
    return @"error";
    
}

// SKPaymentTransactionObserver protocol method
// Sent when the transaction array has changed (additions or state changes).  Client should check state of transactions and finish as appropriate.
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    for (SKPaymentTransaction *transaction in transactions) {
        const char* method = "iap_purchase";
        NSString* request_id = [NSString stringWithFormat:@"%p",transaction.payment];
        NSMutableDictionary* dict = [NSMutableDictionary dictionaryWithCapacity:5];
        [dict setObject:request_id forKey:@"request_id"];
        SB_LOGI("paymentQueue:updatedTransactions: " << transaction.description.UTF8String);
        if (transaction.payment)
            [dict setObject:transaction.payment.productIdentifier forKey:@"product_id"];
        switch (transaction.transactionState) {
                // Call the appropriate custom method for the transaction state.
            case SKPaymentTransactionStatePurchasing:
                //[self showTransactionAsInProgress:transaction deferred:NO];
                [dict setObject:@"purchasing" forKey:@"state"];
                break;
            case SKPaymentTransactionStateDeferred:
                //[self showTransactionAsInProgress:transaction deferred:YES];
                [dict setObject:@"deferred" forKey:@"state"];
                break;
            case SKPaymentTransactionStateFailed:
                //[self failedTransaction:transaction];
                [dict setObject:@"failed" forKey:@"state"];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStatePurchased:
                //[self completeTransaction:transaction];
                [dict setObject:@"purchased" forKey:@"state"];
                [dict setObject:[self getReceiptForTransaction:transaction] forKey:@"transaction"];
                break;
            case SKPaymentTransactionStateRestored:
                method = "iap_restore_payments";
                //[self restoreTransaction:transaction];
                [dict setObject:@"restored" forKey:@"state"];
                [dict setObject:[self getReceiptForTransaction:transaction] forKey:@"transaction"];
                break;
            default:
                // For debugging
                SB_LOGE("Unexpected transaction state: " << transaction.transactionState);
                break;
        }
        if (m_application) {
            NSData* json_encoded = [NSJSONSerialization dataWithJSONObject:dict options:0 error:nil];
            NSString* newStr = [[NSString alloc] initWithData:json_encoded encoding:NSUTF8StringEncoding];
            m_application->OnExtensionResponse(method,newStr.UTF8String);
            [newStr release];
        }
    }
}

//// Sent when transactions are removed from the queue (via finishTransaction:).
//- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray<SKPaymentTransaction *> *)transactions NS_AVAILABLE_IOS(3_0);
//
//// Sent when an error is encountered while adding transactions from the user's purchase history back to the queue.
//- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error NS_AVAILABLE_IOS(3_0);
//
//// Sent when all transactions from the user's purchase history have successfully been added back to the queue.
//- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue NS_AVAILABLE_IOS(3_0);
//
//// Sent when the download state has changed.
//- (void)paymentQueue:(SKPaymentQueue *)queue updatedDownloads:(NSArray<SKDownload *> *)downloads NS_AVAILABLE_IOS(6_0);


@end

iap_platform_extension::iap_platform_extension(iap_manager* mgr) {
    m_mgr = mgr;
}
iap_platform_extension::iap_platform_extension() {
    m_mgr = [[iap_manager alloc] init];
}
iap_platform_extension::~iap_platform_extension() {
    [m_mgr release];
}
void iap_platform_extension::OnAppStarted(Sandbox::Application* app) {
    [m_mgr setApplication:app];
}

bool iap_platform_extension::Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res) {
    if (::strcmp(method, "iap_get_products_info")==0) {
        NSError *err;
        NSArray *productIdentifiers = [NSJSONSerialization
                                       JSONObjectWithData:[NSData dataWithBytes:args length: ::strlen(args)] options:NULL error:&err];
        if (!productIdentifiers) {
            SB_LOGE("failed parse products " << (err ? err.description.UTF8String : ""));
            return false;
        }
        NSString* request_id = [m_mgr reqiestProductsInformation:productIdentifiers];
        if (!request_id) {
            return false;
        }
        res = request_id.UTF8String;
        return true;
    } else if (::strcmp(method, "iap_purchase")==0) {
        NSString* status = 0;
        NSString* request_id = [m_mgr performPayment:[NSString stringWithUTF8String:args] status:&status];
        if (!request_id) {
            if (status) {
                res = status.UTF8String;
            }
            return false;
        }
        res = request_id.UTF8String;
        return true;
    } else if (::strcmp(method, "iap_confirm_transaction")==0) {
        if ([m_mgr confirmTransaction:[NSString stringWithUTF8String:args]]) {
            res = "success";
        } else {
            res = "failed";
        }
        return true;
    }else if (::strcmp(method, "iap_restore_payments")==0) {
        [m_mgr restorePayments];
        res = "success";
        return true;
    }
    return false;
}




extern "C" void *init_iap_extension() {
    static iap_platform_extension __iap_extension;
    return &__iap_extension;
}
