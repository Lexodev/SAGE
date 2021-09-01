/**
 * sage_3dengine.h
 * 
 * SAGE (Small Amiga Game Engine) project
 * 3D engine functions
 * 
 * @author Fabrice Labrador <fabrice.labrador@gmail.com>
 * @version 1.0 August 2021
 */

#include "sage_math.h"
#include "sage_3dengine.h"

/** Transformation matrix */
SAGE_Matrix ObjectMatrix;
SAGE_Matrix CameraMatrix;

/** Temp buffer for calculation */
SAGE_Point tab_world[S3DE_MAX_POINT];
SAGE_Vector tab_nworld[S3DE_MAX_POINT];
SAGE_Point tab_view[S3DE_MAX_POINT];
//SAGE_Color tab_pcolor[S3DE_MAX_POINT];

VOID SAGE_TrMatrix(SAGE_Matrix * matrix, FLOAT px, FLOAT py, FLOAT pz)
{
	SAGE_IdentityMatrix(matrix);
	matrix->m41 = px;
	matrix->m42 = py;
	matrix->m43 = pz;
}

VOID SAGE_RxMatrix(SAGE_Matrix *matrix, FLOAT ax)
{
  FLOAT sin, cos;

  sin = sin(DEGTORAD(ax));
  cos = cos(DEGTORAD(ax));
	SAGE_IdentityMatrix(matrix);
	matrix->m22 = cos;
	matrix->m23 = sin;
	matrix->m32 = -sin;
	matrix->m33 = cos;
}

VOID SAGE_RyMatrix(SAGE_Matrix *matrix, FLOAT ay)
{
  FLOAT sin, cos;

  sin = sin(DEGTORAD(ay));
  cos = cos(DEGTORAD(ay));
	SAGE_IdentityMatrix(matrix);
	matrix->m11 = cos;
	matrix->m13 = -sin;
	matrix->m31 = sin;
	matrix->m33 = cos;
}

VOID SAGE_RzMatrix(SAGE_Matrix *matrix, FLOAT az)
{
  FLOAT sin, cos;

  sin = sin(DEGTORAD(az));
  cos = cos(DEGTORAD(az));
	SAGE_IdentityMatrix(matrix);
	matrix->m11 = cos;
	matrix->m12 = sin;
	matrix->m21 = -sin;
	matrix->m22 = cos;
}

VOID SAGE_TransformCameraMatrix(SAGE_Camera * camera)
{
	LONG ax,ay,az;
	FLOAT px,py,pz;

	ax = -camera->anglex;
	ay = -camera->angley;
	az = -camera->anglez;
	px = -camera->posx;
	py = -camera->posy;
	pz = -camera->posz;
	ax %= G3D_360DEG;	// ax doit �tre positif et inf�rieur � 360
	if (ax < 0)	ax += G3D_360DEG;
	ay %= G3D_360DEG;	// pareil pour ay
	if (ay < 0) ay += G3D_360DEG;
	az %= G3D_360DEG;	// et pour az
	if (az < 0) az += G3D_360DEG;
	CameraMatrix.m11 = Cosinus[ay]*Cosinus[az];
	CameraMatrix.m12 = Cosinus[ay]*Sinus[az];
	CameraMatrix.m13 = -Sinus[ay];
	CameraMatrix.m14 = 0.0;
	CameraMatrix.m21 = (Sinus[ax]*Sinus[ay]*Cosinus[az]) - (Cosinus[ax]*Sinus[az]);
	CameraMatrix.m22 = (Sinus[ax]*Sinus[ay]*Sinus[az]) + (Cosinus[ax]*Cosinus[az]);
	CameraMatrix.m23 = Sinus[ax]*Cosinus[ay];
	CameraMatrix.m24 = 0.0;
	CameraMatrix.m31 = (Cosinus[ax]*Sinus[ay]*Cosinus[az]) + (Sinus[ax]*Sinus[az]);
	CameraMatrix.m32 = (Cosinus[ax]*Sinus[ay]*Sinus[az]) - (Sinus[ax]*Cosinus[az]);
	CameraMatrix.m33 = Cosinus[ax]*Cosinus[ay];
	CameraMatrix.m34 = 0.0;
	CameraMatrix.m41 = ((((px * Cosinus[ay]) + (((py * Sinus[ax]) + (pz * Cosinus[ax])) * Sinus[ay])) * Cosinus[az]) + (((py * Cosinus[ax]) + (pz * -Sinus[ax])) * -Sinus[az]));
	CameraMatrix.m42 = ((((px * Cosinus[ay]) + (((py * Sinus[ax]) + (pz * Cosinus[ax])) * Sinus[ay])) * Sinus[az]) + (((py * Cosinus[ax]) + ( pz * -Sinus[ax])) * Cosinus[az]));
	CameraMatrix.m43 = ((px * -Sinus[ay]) + (((py * Sinus[ax]) + (pz * Cosinus[ax])) * Cosinus[ay]));
	CameraMatrix.m44 = 1.0;
}

// Calcul de la matrice de transformation d'un objet

VOID SAGE_TransformObjectMatrix(SAGE_Object *object)
{
	LONG ax,ay,az;

	ax = object->anglex;
	ay = object->angley;
	az = object->anglez;
	ax %= G3D_360DEG;	// ax doit �tre positif et inf�rieur � 360
	if (ax < 0)	ax += G3D_360DEG;
	ay %= G3D_360DEG;	// pareil pour ay
	if (ay < 0) ay += G3D_360DEG;
	az %= G3D_360DEG;	// et pour az
	if (az < 0) az += G3D_360DEG;
	ObjectMatrix.m11 = Cosinus[ay]*Cosinus[az];
	ObjectMatrix.m12 = Cosinus[ay]*Sinus[az];
	ObjectMatrix.m13 = -Sinus[ay];
	ObjectMatrix.m14 = 0.0;
	ObjectMatrix.m21 = (Sinus[ax]*Sinus[ay]*Cosinus[az]) - (Cosinus[ax]*Sinus[az]);
	ObjectMatrix.m22 = (Sinus[ax]*Sinus[ay]*Sinus[az]) + (Cosinus[ax]*Cosinus[az]);
	ObjectMatrix.m23 = Sinus[ax]*Cosinus[ay];
	ObjectMatrix.m24 = 0.0;
	ObjectMatrix.m31 = (Cosinus[ax]*Sinus[ay]*Cosinus[az]) + (Sinus[ax]*Sinus[az]);
	ObjectMatrix.m32 = (Cosinus[ax]*Sinus[ay]*Sinus[az]) - (Sinus[ax]*Cosinus[az]);
	ObjectMatrix.m33 = Cosinus[ax]*Cosinus[ay];
	ObjectMatrix.m34 = 0.0;
	ObjectMatrix.m41 = object->posx;
	ObjectMatrix.m42 = object->posy;
	ObjectMatrix.m43 = object->posz;
	ObjectMatrix.m44 = 1.0;
}

// Test si l'objet est dans le cone de vision

BOOL SAGE_ObjectCulling(SAGE_Camera *camera,SAGE_Object *object)
{
	SAGE_Vector obj_center;
	SAGE_Vector tr_center;
	FLOAT radius,xplan,yplan;

	obj_center.x = object->posx;
	obj_center.y = object->posy;
	obj_center.z = object->posz;
	SAGE_VectorMatrix4(&tr_center,&obj_center,&CameraMatrix);
	radius = object->radius;
	// on test d'abord par rapport aux plans en Z
	if (((tr_center.z-radius) > camera->far_plane) || ((tr_center.z+radius) < camera->near_plane))
		return(FALSE);	// l'objet est hors du cone
	// on test � pr�sent par rapport aux plans latt�raux
	xplan = (camera->centery*tr_center.z)/camera->focale;
	if (((tr_center.x-radius) > xplan) || ((tr_center.x+radius) < -xplan))
		return(FALSE);
	yplan = (camera->centerx*tr_center.z)/camera->focale;
	if (((tr_center.y-radius) > yplan) || ((tr_center.y+radius) < -yplan))
		return(FALSE);
	if (((tr_center.z+radius) > camera->far_plane) || ((tr_center.z-radius) < camera->near_plane))
		object->clipped = TRUE;	// une partie de l'objet est hors du cone
	else if (((tr_center.x+radius) > xplan) || ((tr_center.x-radius) < -xplan))
		object->clipped = TRUE;
	else if (((tr_center.y+radius) > yplan) || ((tr_center.y-radius) < -yplan))
		object->clipped = TRUE;
	else
		object->clipped = FALSE;	// l'objet est totalement dans le cone
	return(TRUE);
}

// Transforme les coordonn�es de l'objet et les normales
// de chaque point du rep�re local vers le rep�re du monde

VOID SAGE_LocalToWorld(SAGE_Object *object)
{
	UWORD ct_point;
	SAGE_Point *pt3d;
	SAGE_Vector *pn3d;

	pt3d = object->tab_point;
	pn3d = object->tab_pnormal;
	for (ct_point = 0;ct_point < object->nb_point;ct_point++)
	{
		// transforme les coordonn�es du point
		SAGE_VectorMatrix4((SAGE_Vector *)&(tab_world[ct_point]),(SAGE_Vector *)&(pt3d[ct_point]),&ObjectMatrix);
		// transforme les normales des points
		SAGE_VectorMatrix(&(tab_nworld[ct_point]),&(pn3d[ct_point]),&ObjectMatrix);
		// met la couleur du point � 0
		tab_pcolor[ct_point].red = 0.0;
		tab_pcolor[ct_point].green = 0.0;
		tab_pcolor[ct_point].blue = 0.0;
	}
}

// Test les faces cach�es

VOID SAGE_BackFaceCulling(SAGE_Camera *camera,SAGE_Object *object)
{
	UWORD ct_face,p1;
	FLOAT res;
	SAGE_Face *fa3d;
	SAGE_Vector sight,normal;

	// Calcul pour chaque face de l'objet
	fa3d = object->tab_face;
	for (ct_face = 0;ct_face < object->nb_face;ct_face++)
	{
		fa3d[ct_face].clipped = G3D_NOCLIP;
		if (!fa3d[ct_face].visible)
			fa3d[ct_face].culled = TRUE;
		else
		{
			if (fa3d[ct_face].double_sided)
				fa3d[ct_face].culled = FALSE;
			else
			{
				SAGE_VectorMatrix(&normal,&(fa3d[ct_face].normal),&ObjectMatrix);
				p1 = fa3d[ct_face].p1;
				sight.x = camera->posx - tab_world[p1].x;
				sight.y = camera->posy - tab_world[p1].y;
				sight.z = camera->posz - tab_world[p1].z;
				// Produit entre la normale � la face et la ligne de vue
				res = SAGE_DotProduct(&normal,&sight);
				if (res > 0.0)	// si le produit est positif alors la face est visible
					fa3d[ct_face].culled = FALSE;
				else
					fa3d[ct_face].culled = TRUE;
			}
		}
	}
}

/**
VOID SAGE_DirectionalLight(SAGE_Light *light,SAGE_Object *object)
{
	SAGE_Vector *vlight;
	UWORD ct_point;
	FLOAT product;

	// une lumi�re directionnelle est une lumi�re dont on
	// consid�re que les rayons lumineux frappent les objets
	// toujours selon la m�me direction
	vlight = &(light->vector);
	for (ct_point = 0;ct_point < object->nb_point;ct_point++)
	{
		product = SAGE_DotProduct(vlight,&(tab_nworld[ct_point]));
		if (product > 0.0)	// si la lumi�re frappe le point
		{
			tab_pcolor[ct_point].red += product;
			if (tab_pcolor[ct_point].red > G3D_MAX_COLOR)
				tab_pcolor[ct_point].red = G3D_MAX_COLOR;
			tab_pcolor[ct_point].green += product;
			if (tab_pcolor[ct_point].green > G3D_MAX_COLOR)
				tab_pcolor[ct_point].green = G3D_MAX_COLOR;
			tab_pcolor[ct_point].blue += product;
			if (tab_pcolor[ct_point].blue > G3D_MAX_COLOR)
				tab_pcolor[ct_point].blue = G3D_MAX_COLOR;
		}
	}
}
*/

/**
VOID SAGE_PointLight(SAGE_Light *light,SAGE_Object *object)
{
	// A FAIRE
}
*/

/**
VOID SAGE_FaceShading(SAGE_Object *object,SAGE_Light **tab_light,UWORD max_light)
{
	SAGE_Light *light;
	UWORD ct_light;

	for (ct_light = 0;ct_light < max_light;ct_light++)
	{
		light = tab_light[ct_light];
		if (light)
		{
			if (light->type == G3D_DIRECTIONNAL_LIGHT)
				SAGE_DirectionalLight(light,object);
			else
				SAGE_PointLight(light,object);
		}
	}
}
*/

VOID SAGE_WorldToCamera(SAGE_Object *object)
{
	UWORD ct_point;

	for (ct_point = 0;ct_point < object->nb_point;ct_point++)
		SAGE_VectorMatrix4((SAGE_Vector *)&(tab_view[ct_point]),(SAGE_Vector *)&(tab_world[ct_point]),&CameraMatrix);
}

VOID SAGE_FaceClipping(SAGE_Camera *camera,SAGE_Object *object)
{
	UWORD ct_face,p1,p2,p3,p4;
	FLOAT nearp,farp;
	FLOAT x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4;
	SAGE_Face *fa3d;

	nearp = camera->near_plane;
	farp = camera->far_plane;
	// Calcul pour chaque face de l'objet
	fa3d = object->tab_face;
	for (ct_face = 0;ct_face < object->nb_face;ct_face++)
	{
		// On ne fait le calcul que si la face est visible
		if (!fa3d[ct_face].culled)
		{
			p1 = fa3d[ct_face].p1;
			x1 = tab_view[p1].x; y1 = tab_view[p1].y; z1 = tab_view[p1].z;
			p2 = fa3d[ct_face].p2;
			x2 = tab_view[p2].x; y2 = tab_view[p2].y; z2 = tab_view[p2].z;
			p3 = fa3d[ct_face].p3;
			x3 = tab_view[p3].x; y3 = tab_view[p3].y; z3 = tab_view[p3].z;
			if (fa3d[ct_face].quad)
			{
				p4 = fa3d[ct_face].p4;
				x4 = tab_view[p4].x; y4 = tab_view[p4].y; z4 = tab_view[p4].z;
			}
			else
			{
				p4 = p3;
				x4 = x3; y4 = y3; z4 = z3;
			}
			// On test d'abord les faces totalement en dehors du c�ne
			if ((z1<nearp && z2<nearp && z3<nearp && z4<nearp) || (z1>farp && z2>farp && z3>farp && z4>farp))
				fa3d[ct_face].culled = TRUE;
			else
			{
				if ((x1<-z1 && x2<-z2 && x3<-z3 && z4<-z4) || (x1>z1 && x2>z2 && x3>z3 && x4>z4))
					fa3d[ct_face].culled = TRUE;
				else
				{
					if ((y1<-z1 && y2<-z2 && y3<-z3 && y4<-z4) || (y1>z1 && y2>z2 && y3>z3 && y4>z4))
						fa3d[ct_face].culled = TRUE;
					else
					{
						if (z1 < nearp)
							fa3d[ct_face].clipped |= G3D_P1CLIP;
						if (z2 < nearp)
							fa3d[ct_face].clipped |= G3D_P2CLIP;
						if (z3 < nearp)
							fa3d[ct_face].clipped |= G3D_P3CLIP;
						if (z4 < nearp)
							fa3d[ct_face].clipped |= G3D_P4CLIP;
					}
				}
			}
		}
	}
}

UWORD SAGE_SetClipPolyList(SAGE_Object *object,SAGE_Texture **tab_texture,SAGE_Color *ambiant,W3D_Triangle *vf3d,UWORD ct_vface,FLOAT nearp)
{
	FLOAT ared,agreen,ablue,falpha,fred,fgreen,fblue;
	UWORD ct_face,p1,p2,p3,p4;
	SAGE_Face *fa3d;
	SAGE_Clip clip;

	fa3d = object->tab_face;
	ared = ambiant->red;
	agreen = ambiant->green;
	ablue = ambiant->blue;
	// Pour chaque faces de l'objet
	for (ct_face = 0;ct_face < object->nb_face;ct_face++)
	{
		if (!fa3d[ct_face].culled)
		{
			p1 = fa3d[ct_face].p1;
			p2 = fa3d[ct_face].p2;
			p3 = fa3d[ct_face].p3;
			p4 = fa3d[ct_face].p4;
			falpha = fa3d[ct_face].color.alpha;
			fred = fa3d[ct_face].color.red * ared;
			fgreen = fa3d[ct_face].color.green * agreen;
			fblue = fa3d[ct_face].color.blue * ablue;
			if (fa3d[ct_face].clipped == G3D_NOCLIP)
			{
				SAGE_CopyTriangle1(vf3d,ct_vface,p1,p2,p3,fa3d,ct_face,falpha,fred,fgreen,fblue);
				vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
				ct_vface++;
				if (fa3d[ct_face].quad)
				{
					SAGE_CopyTriangle2(vf3d,ct_vface,p1,p3,p4,fa3d,ct_face,falpha,fred,fgreen,fblue);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
				}
			}
			else
			{
				// on commence par s'occuper du premier triangle
				switch (fa3d[ct_face].clipped & G3D_P4MASK)
				{
				case G3D_NOCLIP:	// tous les points sont dans le c�ne
					SAGE_CopyTriangle1(vf3d,ct_vface,p1,p2,p3,fa3d,ct_face,falpha,fred,fgreen,fblue);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				case G3D_P1CLIP:	// seul le point1 est hors du c�ne
					clip.t1 = (nearp - tab_view[p1].z) / (tab_view[p2].z - tab_view[p1].z);
					clip.t2 = (nearp - tab_view[p1].z) / (tab_view[p3].z - tab_view[p1].z);
					clip.u1 = fa3d[ct_face].u1; clip.v1 = fa3d[ct_face].v1;
					clip.u2 = fa3d[ct_face].u2; clip.v2 = fa3d[ct_face].v2;
					clip.u3 = fa3d[ct_face].u3; clip.v3 = fa3d[ct_face].v3;
					SAGE_ClipTriangle1(vf3d,ct_vface,&clip,p1,p2,p3,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				case G3D_P2CLIP:	// seul le point2 est hors du c�ne
					clip.t1 = (nearp - tab_view[p2].z) / (tab_view[p1].z - tab_view[p2].z);
					clip.t2 = (nearp - tab_view[p2].z) / (tab_view[p3].z - tab_view[p2].z);
					clip.u1 = fa3d[ct_face].u2; clip.v1 = fa3d[ct_face].v2;
					clip.u2 = fa3d[ct_face].u1; clip.v2 = fa3d[ct_face].v1;
					clip.u3 = fa3d[ct_face].u3; clip.v3 = fa3d[ct_face].v3;
					SAGE_ClipTriangle1(vf3d,ct_vface,&clip,p2,p1,p3,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				case G3D_P3CLIP:	// seul le point3 est hors du c�ne
					clip.t1 = (nearp - tab_view[p3].z) / (tab_view[p1].z - tab_view[p3].z);
					clip.t2 = (nearp - tab_view[p3].z) / (tab_view[p2].z - tab_view[p3].z);
					clip.u1 = fa3d[ct_face].u3; clip.v1 = fa3d[ct_face].v3;
					clip.u2 = fa3d[ct_face].u1; clip.v2 = fa3d[ct_face].v1;
					clip.u3 = fa3d[ct_face].u2; clip.v3 = fa3d[ct_face].v2;
					SAGE_ClipTriangle1(vf3d,ct_vface,&clip,p3,p1,p2,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				case G3D_P1CLIP|G3D_P2CLIP:
					clip.t1 = (nearp - tab_view[p1].z) / (tab_view[p3].z - tab_view[p1].z);
					clip.t2 = (nearp - tab_view[p2].z) / (tab_view[p3].z - tab_view[p2].z);
					clip.u1 = fa3d[ct_face].u1; clip.v1 = fa3d[ct_face].v1;
					clip.u2 = fa3d[ct_face].u2; clip.v2 = fa3d[ct_face].v2;
					clip.u3 = fa3d[ct_face].u3; clip.v3 = fa3d[ct_face].v3;
					SAGE_ClipTriangle2(vf3d,ct_vface,&clip,p1,p2,p3,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				case G3D_P1CLIP|G3D_P3CLIP:
					clip.t1 = (nearp - tab_view[p1].z) / (tab_view[p2].z - tab_view[p1].z);
					clip.t2 = (nearp - tab_view[p3].z) / (tab_view[p2].z - tab_view[p3].z);
					clip.u1 = fa3d[ct_face].u1; clip.v1 = fa3d[ct_face].v1;
					clip.u2 = fa3d[ct_face].u3; clip.v2 = fa3d[ct_face].v3;
					clip.u3 = fa3d[ct_face].u2; clip.v3 = fa3d[ct_face].v2;
					SAGE_ClipTriangle2(vf3d,ct_vface,&clip,p1,p3,p2,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				case G3D_P2CLIP|G3D_P3CLIP:
					clip.t1 = (nearp - tab_view[p2].z) / (tab_view[p1].z - tab_view[p2].z);
					clip.t2 = (nearp - tab_view[p3].z) / (tab_view[p1].z - tab_view[p3].z);
					clip.u1 = fa3d[ct_face].u2; clip.v1 = fa3d[ct_face].v1;
					clip.u2 = fa3d[ct_face].u3; clip.v2 = fa3d[ct_face].v2;
					clip.u3 = fa3d[ct_face].u1; clip.v3 = fa3d[ct_face].v1;
					SAGE_ClipTriangle2(vf3d,ct_vface,&clip,p2,p3,p1,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
					vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
					ct_vface++;
					break;
				default:	// tous les points sont hors du c�ne
					break;
				}
				if (fa3d[ct_face].quad)
				{
					// on passe au deuxi�me triangle
					switch (fa3d[ct_face].clipped & G3D_P2MASK)
					{
					case G3D_NOCLIP:	// tous les points sont dans le c�ne
						SAGE_CopyTriangle2(vf3d,ct_vface,p1,p3,p4,fa3d,ct_face,falpha,fred,fgreen,fblue);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					case G3D_P1CLIP:	// seul le point1 est hors du c�ne
						clip.t1 = (nearp - tab_view[p1].z) / (tab_view[p3].z - tab_view[p1].z);
						clip.t2 = (nearp - tab_view[p1].z) / (tab_view[p4].z - tab_view[p1].z);
						clip.u1 = fa3d[ct_face].u1; clip.v1 = fa3d[ct_face].v1;
						clip.u2 = fa3d[ct_face].u3; clip.v2 = fa3d[ct_face].v3;
						clip.u3 = fa3d[ct_face].u4; clip.v3 = fa3d[ct_face].v4;
						SAGE_ClipTriangle1(vf3d,ct_vface,&clip,p1,p3,p4,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					case G3D_P3CLIP:	// seul le point3 est hors du c�ne
						clip.t1 = (nearp - tab_view[p3].z) / (tab_view[p1].z - tab_view[p3].z);
						clip.t2 = (nearp - tab_view[p3].z) / (tab_view[p4].z - tab_view[p3].z);
						clip.u1 = fa3d[ct_face].u3; clip.v1 = fa3d[ct_face].v3;
						clip.u2 = fa3d[ct_face].u1; clip.v2 = fa3d[ct_face].v1;
						clip.u3 = fa3d[ct_face].u4; clip.v3 = fa3d[ct_face].v4;
						SAGE_ClipTriangle1(vf3d,ct_vface,&clip,p3,p1,p4,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					case G3D_P4CLIP:	// seul le point4 est hors du c�ne
						clip.t1 = (nearp - tab_view[p4].z) / (tab_view[p1].z - tab_view[p4].z);
						clip.t2 = (nearp - tab_view[p4].z) / (tab_view[p3].z - tab_view[p4].z);
						clip.u1 = fa3d[ct_face].u4; clip.v1 = fa3d[ct_face].v4;
						clip.u2 = fa3d[ct_face].u1; clip.v2 = fa3d[ct_face].v1;
						clip.u3 = fa3d[ct_face].u3; clip.v3 = fa3d[ct_face].v3;
						SAGE_ClipTriangle1(vf3d,ct_vface,&clip,p4,p1,p3,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					case G3D_P1CLIP|G3D_P3CLIP:
						clip.t1 = (nearp - tab_view[p1].z) / (tab_view[p4].z - tab_view[p1].z);
						clip.t2 = (nearp - tab_view[p3].z) / (tab_view[p4].z - tab_view[p3].z);
						clip.u1 = fa3d[ct_face].u1; clip.v1 = fa3d[ct_face].v1;
						clip.u2 = fa3d[ct_face].u3; clip.v2 = fa3d[ct_face].v3;
						clip.u3 = fa3d[ct_face].u4; clip.v3 = fa3d[ct_face].v4;
						SAGE_ClipTriangle2(vf3d,ct_vface,&clip,p1,p3,p4,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					case G3D_P1CLIP|G3D_P4CLIP:
						clip.t1 = (nearp - tab_view[p1].z) / (tab_view[p3].z - tab_view[p1].z);
						clip.t2 = (nearp - tab_view[p4].z) / (tab_view[p3].z - tab_view[p4].z);
						clip.u1 = fa3d[ct_face].u1; clip.v1 = fa3d[ct_face].v1;
						clip.u2 = fa3d[ct_face].u4; clip.v2 = fa3d[ct_face].v4;
						clip.u3 = fa3d[ct_face].u3; clip.v3 = fa3d[ct_face].v3;
						SAGE_ClipTriangle2(vf3d,ct_vface,&clip,p1,p4,p3,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					case G3D_P3CLIP|G3D_P4CLIP:
						clip.t1 = (nearp - tab_view[p3].z) / (tab_view[p1].z - tab_view[p3].z);
						clip.t2 = (nearp - tab_view[p4].z) / (tab_view[p1].z - tab_view[p4].z);
						clip.u1 = fa3d[ct_face].u3; clip.v1 = fa3d[ct_face].v3;
						clip.u2 = fa3d[ct_face].u4; clip.v2 = fa3d[ct_face].v4;
						clip.u3 = fa3d[ct_face].u1; clip.v3 = fa3d[ct_face].v1;
						SAGE_ClipTriangle2(vf3d,ct_vface,&clip,p3,p4,p1,fa3d,ct_face,falpha,fred,fgreen,fblue,nearp);
						vf3d[ct_vface].tex = tab_texture[fa3d[ct_face].idx_texture]->texture;
						ct_vface++;
						break;
					default:	// tous les points sont hors du c�ne
						break;
					}
				}
			}
		}
	}
	return(ct_vface);
}

// Copie le premier triangle de la face

VOID SAGE_CopyTriangle1(W3D_Triangle *vf3d,UWORD ct_vface,UWORD p1,UWORD p2,UWORD p3,SAGE_Face *fa3d,UWORD ct_face,FLOAT falpha,FLOAT fred,FLOAT fgreen,FLOAT fblue)
{
	FLOAT red1,green1,blue1,red2,green2,blue2,red3,green3,blue3;

	vf3d[ct_vface].v1.x = tab_view[p1].x;
	vf3d[ct_vface].v1.y = tab_view[p1].y;
	vf3d[ct_vface].v1.z = tab_view[p1].z;
	vf3d[ct_vface].v1.w = 1.0 / tab_view[p1].z;
	vf3d[ct_vface].v1.u = fa3d[ct_face].u1;
	vf3d[ct_vface].v1.v = fa3d[ct_face].v1;
	vf3d[ct_vface].v1.color.a = falpha;
	red1 = fred + tab_pcolor[p1].red;
	if (red1 > G3D_MAX_COLOR) red1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.r = red1;
	green1 = fgreen + tab_pcolor[p1].green;
	if (green1 > G3D_MAX_COLOR) green1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.g = green1;
	blue1 = fblue + tab_pcolor[p1].blue;
	if (blue1 > G3D_MAX_COLOR) blue1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.b = blue1;
	vf3d[ct_vface].v2.x = tab_view[p2].x;
	vf3d[ct_vface].v2.y = tab_view[p2].y;
	vf3d[ct_vface].v2.z = tab_view[p2].z;
	vf3d[ct_vface].v2.w = 1.0 / tab_view[p2].z;
	vf3d[ct_vface].v2.u = fa3d[ct_face].u2;
	vf3d[ct_vface].v2.v = fa3d[ct_face].v2;
	vf3d[ct_vface].v2.color.a = falpha;
	red2 = fred + tab_pcolor[p2].red;
	if (red2 >= G3D_MAX_COLOR) red2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.r = red2;
	green2 = fgreen + tab_pcolor[p2].green;
	if (green2 > G3D_MAX_COLOR) green2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.g = green2;
	blue2 = fblue + tab_pcolor[p2].blue;
	if (blue2 > G3D_MAX_COLOR) blue2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.b = blue2;
	vf3d[ct_vface].v3.x = tab_view[p3].x;
	vf3d[ct_vface].v3.y = tab_view[p3].y;
	vf3d[ct_vface].v3.z = tab_view[p3].z;
	vf3d[ct_vface].v3.w = 1.0 / tab_view[p3].z;
	vf3d[ct_vface].v3.u = fa3d[ct_face].u3;
	vf3d[ct_vface].v3.v = fa3d[ct_face].v3;
	vf3d[ct_vface].v3.color.a = falpha;
	red3 = fred + tab_pcolor[p3].red;
	if (red3 > G3D_MAX_COLOR) red3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.r = red3;
	green3 = fgreen + tab_pcolor[p3].green;
	if (green3 > G3D_MAX_COLOR) green3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.g = green3;
	blue3 = fblue + tab_pcolor[p3].blue;
	if (blue3 > G3D_MAX_COLOR) blue3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.b = blue3;
}

// Copie le second triangle de la face

VOID SAGE_CopyTriangle2(W3D_Triangle *vf3d,UWORD ct_vface,UWORD p1,UWORD p3,UWORD p4,SAGE_Face *fa3d,UWORD ct_face,FLOAT falpha,FLOAT fred,FLOAT fgreen,FLOAT fblue)
{
	FLOAT red1,green1,blue1,red3,green3,blue3,red4,green4,blue4;

	vf3d[ct_vface].v1.x = tab_view[p1].x;
	vf3d[ct_vface].v1.y = tab_view[p1].y;
	vf3d[ct_vface].v1.z = tab_view[p1].z;
	vf3d[ct_vface].v1.w = 1.0 / tab_view[p1].z;
	vf3d[ct_vface].v1.u = fa3d[ct_face].u1;
	vf3d[ct_vface].v1.v = fa3d[ct_face].v1;
	vf3d[ct_vface].v1.color.a = falpha;
	red1 = fred + tab_pcolor[p1].red;
	if (red1 > G3D_MAX_COLOR) red1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.r = red1;
	green1 = fgreen + tab_pcolor[p1].green;
	if (green1 > G3D_MAX_COLOR) green1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.g = green1;
	blue1 = fblue + tab_pcolor[p1].blue;
	if (blue1 > G3D_MAX_COLOR) blue1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.b = blue1;
	vf3d[ct_vface].v2.x = tab_view[p3].x;
	vf3d[ct_vface].v2.y = tab_view[p3].y;
	vf3d[ct_vface].v2.z = tab_view[p3].z;
	vf3d[ct_vface].v2.w = 1.0 / tab_view[p3].z;
	vf3d[ct_vface].v2.u = fa3d[ct_face].u3;
	vf3d[ct_vface].v2.v = fa3d[ct_face].v3;
	vf3d[ct_vface].v2.color.a = falpha;
	red3 = fred + tab_pcolor[p3].red;
	if (red3 > G3D_MAX_COLOR) red3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.r = red3;
	green3 = fgreen + tab_pcolor[p3].green;
	if (green3 > G3D_MAX_COLOR) green3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.g = green3;
	blue3 = fblue + tab_pcolor[p3].blue;
	if (blue3 > G3D_MAX_COLOR) blue3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.b = blue3;
	vf3d[ct_vface].v3.x = tab_view[p4].x;
	vf3d[ct_vface].v3.y = tab_view[p4].y;
	vf3d[ct_vface].v3.z = tab_view[p4].z;
	vf3d[ct_vface].v3.w = 1.0 / tab_view[p4].z;
	vf3d[ct_vface].v3.u = fa3d[ct_face].u4;
	vf3d[ct_vface].v3.v = fa3d[ct_face].v4;
	vf3d[ct_vface].v3.color.a = falpha;
	red4 = fred + tab_pcolor[p4].red;
	if (red4 >= G3D_MAX_COLOR) red4 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.r = red4;
	green4 = fgreen + tab_pcolor[p4].green;
	if (green4 > G3D_MAX_COLOR) green4 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.g = green4;
	blue4 = fblue + tab_pcolor[p4].blue;
	if (blue4 > G3D_MAX_COLOR) blue4 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.b = blue4;
}

// Clip un triangle de la face ( lorsqu'un seul point est hors du c�ne )

VOID SAGE_ClipTriangle1(W3D_Triangle *vf3d,UWORD ct_vface,SAGE_Clip *clip,UWORD p1,UWORD p2,UWORD p3,SAGE_Face *fa3d,UWORD ct_face,FLOAT falpha,FLOAT fred,FLOAT fgreen,FLOAT fblue,FLOAT nearp)
{
	FLOAT red1,green1,blue1,red2,green2,blue2,red3,green3,blue3;
	FLOAT xp,yp,up,vp,xs,ys,us,vs;

	xp = tab_view[p1].x + (tab_view[p2].x - tab_view[p1].x) * clip->t1;
	yp = tab_view[p1].y + (tab_view[p2].y - tab_view[p1].y) * clip->t1;
	up = clip->u1 + (clip->u2 - clip->u1) * clip->t1;
	vp = clip->v1 + (clip->v2 - clip->v1) * clip->t1;

	vf3d[ct_vface].v1.x = xp;
	vf3d[ct_vface].v1.y = yp;
	vf3d[ct_vface].v1.z = nearp;
	vf3d[ct_vface].v1.w = 1.0 / nearp;
	vf3d[ct_vface].v1.u = up;
	vf3d[ct_vface].v1.v = vp;
	vf3d[ct_vface].v1.color.a = falpha;
	red1 = fred + tab_pcolor[p1].red;
	if (red1 >= G3D_MAX_COLOR) red1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.r = red1;
	green1 = fgreen + tab_pcolor[p1].green;
	if (green1 >= G3D_MAX_COLOR) green1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.g = green1;
	blue1 = fblue + tab_pcolor[p1].blue;
	if (blue1 >= G3D_MAX_COLOR) blue1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.b = blue1;
	vf3d[ct_vface].v2.x = tab_view[p2].x;
	vf3d[ct_vface].v2.y = tab_view[p2].y;
	vf3d[ct_vface].v2.z = tab_view[p2].z;
	vf3d[ct_vface].v2.w = 1.0 / tab_view[p2].z;
	vf3d[ct_vface].v2.u = clip->u2;
	vf3d[ct_vface].v2.v = clip->v2;
	vf3d[ct_vface].v2.color.a = falpha;
	red2 = fred + tab_pcolor[p2].red;
	if (red2 > G3D_MAX_COLOR) red2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.r = red2;
	green2 = fgreen + tab_pcolor[p2].green;
	if (green2 > G3D_MAX_COLOR) green2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.g = green2;
	blue2 = fblue + tab_pcolor[p2].blue;
	if (blue2 > G3D_MAX_COLOR) blue2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.b = blue2;
	vf3d[ct_vface].v3.x = tab_view[p3].x;
	vf3d[ct_vface].v3.y = tab_view[p3].y;
	vf3d[ct_vface].v3.z = tab_view[p3].z;
	vf3d[ct_vface].v3.w = 1.0 / tab_view[p3].z;
	vf3d[ct_vface].v3.u = clip->u3;
	vf3d[ct_vface].v3.v = clip->v3;
	vf3d[ct_vface].v3.color.a = falpha;
	red3 = fred + tab_pcolor[p3].red;
	if (red3 > G3D_MAX_COLOR) red3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.r = red3;
	green3 = fgreen + tab_pcolor[p3].green;
	if (green3 > G3D_MAX_COLOR) green3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.g = green3;
	blue3 = fblue + tab_pcolor[p3].blue;
	if (blue3 > G3D_MAX_COLOR) blue3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.b = blue3;

	ct_vface++;

	xs = tab_view[p1].x + (tab_view[p3].x - tab_view[p1].x) * clip->t2;
	ys = tab_view[p1].y + (tab_view[p3].y - tab_view[p1].y) * clip->t2;
	us = clip->u1 + (clip->u3 - clip->u1) * clip->t2;
	vs = clip->v1 + (clip->v3 - clip->v1) * clip->t2;

	vf3d[ct_vface].v1.x = xp;
	vf3d[ct_vface].v1.y = yp;
	vf3d[ct_vface].v1.z = nearp;
	vf3d[ct_vface].v1.w = 1.0 / nearp;
	vf3d[ct_vface].v1.u = up;
	vf3d[ct_vface].v1.v = vp;
	vf3d[ct_vface].v1.color.a = falpha;
	vf3d[ct_vface].v1.color.r = red1;
	vf3d[ct_vface].v1.color.g = green1;
	vf3d[ct_vface].v1.color.b = blue1;
	vf3d[ct_vface].v2.x = xs;
	vf3d[ct_vface].v2.y = ys;
	vf3d[ct_vface].v2.z = nearp;
	vf3d[ct_vface].v2.w = 1.0 / nearp;
	vf3d[ct_vface].v2.u = us;
	vf3d[ct_vface].v2.v = vs;
	vf3d[ct_vface].v2.color.a = falpha;
	vf3d[ct_vface].v2.color.r = red1;
	vf3d[ct_vface].v2.color.g = green1;
	vf3d[ct_vface].v2.color.b = blue1;
	vf3d[ct_vface].v3.x = tab_view[p3].x;
	vf3d[ct_vface].v3.y = tab_view[p3].y;
	vf3d[ct_vface].v3.z = tab_view[p3].z;
	vf3d[ct_vface].v3.w = 1.0 / tab_view[p3].z;
	vf3d[ct_vface].v3.u = clip->u3;
	vf3d[ct_vface].v3.v = clip->v3;
	vf3d[ct_vface].v3.color.a = falpha;
	vf3d[ct_vface].v3.color.r = red3;
	vf3d[ct_vface].v3.color.g = green3;
	vf3d[ct_vface].v3.color.b = blue3;
}

// Clip un triangle de la face ( lorsque deux point sont hors du c�ne )

VOID SAGE_ClipTriangle2(W3D_Triangle *vf3d,UWORD ct_vface,SAGE_Clip *clip,UWORD p1,UWORD p2,UWORD p3,SAGE_Face *fa3d,UWORD ct_face,FLOAT falpha,FLOAT fred,FLOAT fgreen,FLOAT fblue,FLOAT nearp)
{
	FLOAT red1,green1,blue1,red2,green2,blue2,red3,green3,blue3;

	vf3d[ct_vface].v1.x = tab_view[p1].x + (tab_view[p3].x - tab_view[p1].x) * clip->t1;
	vf3d[ct_vface].v1.y = tab_view[p1].y + (tab_view[p2].y - tab_view[p1].y) * clip->t1;
	vf3d[ct_vface].v1.z = nearp;
	vf3d[ct_vface].v1.w = 1.0 / nearp;
	vf3d[ct_vface].v1.u = clip->u1 + (clip->u2 - clip->u1) * clip->t1;
	vf3d[ct_vface].v1.v = clip->v1 + (clip->v2 - clip->v1) * clip->t1;
	vf3d[ct_vface].v1.color.a = falpha;
	red1 = fred + tab_pcolor[p1].red;
	if (red1 >= G3D_MAX_COLOR) red1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.r = red1;
	green1 = fgreen + tab_pcolor[p1].green;
	if (green1 >= G3D_MAX_COLOR) green1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.g = green1;
	blue1 = fblue + tab_pcolor[p1].blue;
	if (blue1 >= G3D_MAX_COLOR) blue1 = G3D_MAX_COLOR;
	vf3d[ct_vface].v1.color.b = blue1;
	vf3d[ct_vface].v2.x = tab_view[p2].x + (tab_view[p3].x - tab_view[p2].x) * clip->t2;
	vf3d[ct_vface].v2.y = tab_view[p2].y + (tab_view[p3].y - tab_view[p2].y) * clip->t2;
	vf3d[ct_vface].v2.z = nearp;
	vf3d[ct_vface].v2.w = 1.0 / nearp;
	vf3d[ct_vface].v2.u = clip->u2 + (clip->u3 - clip->u2) * clip->t2;
	vf3d[ct_vface].v2.v = clip->v2 + (clip->v3 - clip->v2) * clip->t2;
	vf3d[ct_vface].v2.color.a = falpha;
	red2 = fred + tab_pcolor[p2].red;
	if (red2 > G3D_MAX_COLOR) red2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.r = red2;
	green2 = fgreen + tab_pcolor[p2].green;
	if (green2 > G3D_MAX_COLOR) green2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.g = green2;
	blue2 = fblue + tab_pcolor[p2].blue;
	if (blue2 > G3D_MAX_COLOR) blue2 = G3D_MAX_COLOR;
	vf3d[ct_vface].v2.color.b = blue2;
	vf3d[ct_vface].v3.x = tab_view[p3].x;
	vf3d[ct_vface].v3.y = tab_view[p3].y;
	vf3d[ct_vface].v3.z = tab_view[p3].z;
	vf3d[ct_vface].v3.w = 1.0 / tab_view[p3].z;
	vf3d[ct_vface].v3.u = clip->u3;
	vf3d[ct_vface].v3.v = clip->v3;
	vf3d[ct_vface].v3.color.a = falpha;
	red3 = fred + tab_pcolor[p3].red;
	if (red3 > G3D_MAX_COLOR) red3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.r = red3;
	green3 = fgreen + tab_pcolor[p3].green;
	if (green3 > G3D_MAX_COLOR) green3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.g = green3;
	blue3 = fblue + tab_pcolor[p3].blue;
	if (blue3 > G3D_MAX_COLOR) blue3 = G3D_MAX_COLOR;
	vf3d[ct_vface].v3.color.b = blue3;
}

VOID SAGE_Debug(BOOL debug)
{
	SAGE_debug = debug;
}
