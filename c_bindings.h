#ifndef __CBINDINGS_H
#define __CBINDINGS_H

#ifdef __cplusplus 
extern "C" { 
#endif

/* 
 * Get option or NULL if not found in psaferc 
 */
const char* pwsafe_get_config(const char* option);

/*
 * Gets password or NULL if no password exists with that title
 */
const char* pwsafe_get_password(const char* masterpass, const char* title);

#ifdef __cplusplus 
}
#endif
#endif 
