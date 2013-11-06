/* écran de veille 3dMorph */

/* fonction de dessin */
/* incluse un certain nombre de fois dans Draw3D.cpp */

// il faut définir les paramètres suivants:
// FUNCTION: nom de la fonction
// NPIXELS: nombre de pixels par point
// TYPEPIXEL: type de chaque pixel
FUNCTION
{
	TYPEPIXEL *bits = (TYPEPIXEL*)dwBits;
	int32 rowInts = dwRowBytes/sizeof(TYPEPIXEL);
	
	// double buffering: si on dessine dans le second buffer, il faut déplacer le pointeur
	if ((doubleBuffering && !preview && !softwareDouble) && (frameNum & 1))
		bits+=rowInts*height;
	
	// tout d'abord, on efface les points précédents
	/// détermination des caractéristiques	
	unsigned int nP;	// nombre de points
	BPoint *pts;		// emplacements de ces points
	bool *clp;			// clipping de ces points

	/// si on est en mode double buffering, il faut prendre les points
	/// d'avant les points précédents
	if (doubleBuffering && !preview && !softwareDouble)
	{
		nP = numVoldPoints;
		pts = pVoldPoints;
		clp = dwOldDoClip;
	}
	else
	{
		nP = numOldPoints;
		pts = pOldPoints;
		clp = dwDoClip;	
	}
	
	/// on procède à l'effacement des points
	int32 tx = dwtx;
	int32 ty = dwty;
	if (nP != 0)
		for (unsigned int i=0; i<nP; i++)
			if (clp[i])
			{
				int32 x = (int32)pts[i].x+tx, y = ty-(int32)pts[i].y;
				TYPEPIXEL *pt = (TYPEPIXEL*)(bits+y*rowInts+x);
				*pt=0;
				*(pt-1)=0;
				*(pt+1)=0;
				*(pt-rowInts)=0;
				*(pt+rowInts)=0;
#if NPIXELS == 9
				*(pt-rowInts-1)=0;
				*(pt-rowInts+1)=0;
				*(pt+rowInts-1)=0;
				*(pt+rowInts+1)=0;
#endif
			}

	// on a plus besoin des anciens clippings de points
	delete dwOldDoClip;
	dwOldDoClip = dwDoClip;
	dwDoClip = new bool[numNewPoints];
	
	// dessin des nouveaux points
	// mise à l'échelle du paramètre depth
#if NPIXELS == 5
	depth *= 0.6;
#else
	depth *= 0.5;
#endif
	if (numNewPoints != 0)
	{
		/// détermination de leur visibilité et déssin du tour
		for (unsigned int i=0; i<numNewPoints; i++)
		{
			//// détermination de la visibilité du point
			bool out = false;
			int32 x = (int32)pNewPoints[i].x+tx, y = ty-(int32)pNewPoints[i].y;
			
			///// boucle dans tous les éventuels rectangles de clipping
			for (int j=0; j<dwClipNum && !out; j++)
				if ((x > dwClipList[j].left) &&
					(x < dwClipList[j].right) &&
					(y > dwClipList[j].top) &&
					(y < dwClipList[j].bottom))
					out = true;
			
			//// dessin du tour du point
			if (out && !preview)
			{
				TYPEPIXEL *pt = (TYPEPIXEL*)(bits+y*rowInts+x);
#if NPIXELS == 5
				float ccoef = 0.5+depth*POINTZ(i);
#else
				float ccoef = (1-depth+depth*POINTZ(i))*0.5;
#endif
				if (ccoef>1)
					ccoef = 1;
				else if (ccoef<0)
					ccoef = 0;
										
				TYPEPIXEL dcol;
				convert((uint8)(pointsColor[i].red*ccoef),
								(uint8)(pointsColor[i].green*ccoef),
								(uint8)(pointsColor[i].blue*ccoef),&dcol);
#if NPIXELS == 5
				*(pt-1)=dcol;
				*(pt+1)=dcol;
				*(pt-rowInts)=dcol;
				*(pt+rowInts)=dcol;
#else
				*(pt-rowInts-1)=Max(dcol,*(pt-rowInts-1));
				*(pt-rowInts+1)=Max(dcol,*(pt-rowInts+1));
				*(pt+rowInts-1)=Max(dcol,*(pt+rowInts-1));
				*(pt+rowInts+1)=Max(dcol,*(pt+rowInts+1));

				ccoef = 1-depth+depth*POINTZ(i);
				if (ccoef>1)
					ccoef = 1;
				else if (ccoef<0)
					ccoef = 0;
				convert((uint8)(pointsColor[i].red*ccoef),
								(uint8)(pointsColor[i].green*ccoef),
								(uint8)(pointsColor[i].blue*ccoef),&dcol);
				*(pt-1)=Max(dcol,*(pt-1));
				*(pt+1)=Max(dcol,*(pt+1));
				*(pt-rowInts)=Max(dcol,*(pt-rowInts));
				*(pt+rowInts)=Max(dcol,*(pt+rowInts));
#endif
			}
			dwDoClip[i] = out;
		}
		
		/// dessin des points eux-mêmes					
		for (unsigned int i=0; i<numNewPoints; i++)
		{
			if (dwDoClip[i])
			{
#if NPIXELS == 5
				float ccoef = 1+depth*POINTZ(i);
#else
				float ccoef = 1-depth+depth*POINTZ(i);
#endif
				if (ccoef>1)
					ccoef = 1;
				else if (ccoef<0)
					ccoef = 0;
										
				int32 x = (int32)pNewPoints[i].x+tx, y = ty-(int32)pNewPoints[i].y;

#if NPIXELS == 5				
				TYPEPIXEL *pt=bits+y*rowInts+x;
				convert((uint8)(pointsColor[i].red*ccoef),
					(uint8)(pointsColor[i].green*ccoef),
					(uint8)(pointsColor[i].blue*ccoef),pt);
#else
				TYPEPIXEL pt;
				convert((uint8)(pointsColor[i].red*ccoef),
					(uint8)(pointsColor[i].green*ccoef),
					(uint8)(pointsColor[i].blue*ccoef),&pt);
				*(bits+y*rowInts+x) = Max(pt,*(bits+y*rowInts+x));
#endif				
			}
		}
	}
}