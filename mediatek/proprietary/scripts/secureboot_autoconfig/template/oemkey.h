#ifndef __OEMKEY__
#define __OEMKEY__

/* OEM_PUBK is set as the same as MTEE public key for convenience, but they can be different */
/* OEM_PUBK will be used to verify oemkeystore, and use oemkeystore to verify images */
#define OEM_PUBK_SZ 256

#define OEM_PUBK          REPLACE_VERIFIED_RSA_N

#endif /* __OEMKEY__ */
