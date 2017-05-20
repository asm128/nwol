#ifndef NWOL_SAFE_H_027409278349234
#define NWOL_SAFE_H_027409278349234

#define safe_free(p)						if(p) { free				(p); p=0; }
#define safe_nwol_free(p)					if(p) { ::nwol::nwol_free	(p); p=0; }
#define safe_delete(p)						if(p) { delete				(p); p=0; }
#define safe_closesocket(p)					if(p) { closesocket			(p); p=0; }
#define safe_fclose(p)						if(p) { fclose				(p); p=0; }

#endif // NWOL_SAFE_H_027409278349234