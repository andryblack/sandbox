#ifndef SB_IOS_IAP_EXTENSION_H_INCLUDED
#define SB_IOS_IAP_EXTENSION_H_INCLUDED

namespace Sandbox {
    class Application;
}

#ifdef __OBJC__
#import <StoreKit/StoreKit.h>
@interface iap_manager : NSObject<SKProductsRequestDelegate,SKPaymentTransactionObserver> 
-(SKProduct*)getProductWithIdentifier:(NSString*)identifier;
-(NSString*) reqiestProductsInformation:(NSArray*)products;
-(BOOL)confirmTransaction:(NSString*)transactionData error:(NSString**) error;
-(void)finishTransaction:(SKPaymentTransaction*) transaction;
@end
#else
struct iap_manager;
#endif

#include "sb_ios_extension.h"

class iap_platform_extension : public Sandbox::iOSPlatformExtension {
private:
    iap_manager* m_mgr;
protected:
    explicit iap_platform_extension( iap_manager* mgr );
public:
    iap_platform_extension();
    ~iap_platform_extension();
    virtual void OnAppStarted(Sandbox::Application* app);
    virtual bool Process(Sandbox::Application* app,
                         const char* method,
                         const char* args,
                         sb::string& res);
};


#endif /*SB_IOS_IAP_EXTENSION_H_INCLUDED*/
