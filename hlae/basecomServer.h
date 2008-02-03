typedef bool (* HlaeBcOnRecieve_t)(void *lpClassPointer,unsigned long dwData,unsigned long cbData,void *lpData);

bool HlaeBcSrvStart(unsigned long ulInstance,void *lpGameWindow);
bool HlaeBcSrvStop();