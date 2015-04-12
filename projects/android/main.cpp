

int StartApplication(int argc,char** argv);

extern "C" __attribute__ ((visibility ("default"))) int ghl_android_app_main(int argc,char** argv) {
    StartApplication(argc,argv);
}