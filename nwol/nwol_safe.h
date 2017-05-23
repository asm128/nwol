#ifndef NWOL_SAFE_H_027409278349234
#define NWOL_SAFE_H_027409278349234

#ifndef safe_assign
#	define		safe_assign(p, val)							if(p)						*(p) = val
#endif 
#ifndef safe_free
#	define		safe_free(p)								if(p)						{ free				(p); p = 0; }
#endif 
#ifndef safe_delete
#	define		safe_delete(p)								if(p)						{ delete			(p); p = 0; }
#endif 
#ifndef safe_fclose
#	define		safe_fclose(p)								if(p)						{ fclose			(p); p = 0; }
#endif 
#ifndef safe_closesocket
#	define		safe_closesocket(p)							if(p != INVALID_SOCKET)		{ closesocket		(p); p = INVALID_SOCKET; }
#endif 

#define safe_nwol_free(p)								if(p)						{ ::nwol::nwol_free	(p); p = 0; }

#endif // NWOL_SAFE_H_027409278349234