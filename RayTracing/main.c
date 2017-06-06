#include "rhs_model.h"
#include "rhs_polygonset.h"

#include "EasyBMP.h"
#include "EasyBMP.cpp"
#include <string.h>
char Spline[15]; // spline type can be "BSPLINE" or "CATMULL_ROM"
static int lod[10]={1,2,5,10,20};
static int lod_idx=2;
static double INF = 100000000;
int model_cnt;
MODEL model[20];
PolygonSet cube, spring, totalPS;
GLfloat lightPos0[] = { 0.0f, 20.0f, 30.0f, 1.0f };
GLfloat lightPos1[] = { 70.0f, 10.0f, -30.0f, 1.0f };
GLfloat lightPos2[] = { 30.0f, 0.0f, -30.0f, 1.0f };

GLfloat diffuse0[] = { 1.0f,1.0f,1.0f,1.0f};
GLfloat diffuse1[] = { 0.9f,0.9f,0.9f,1.0f};

GLfloat ambient0[] = { 1.0f,1.0f,1.0f,1.0f};
GLfloat ambient1[] = { 0.9f,0.9f,0.9f,1.0f};

GLfloat specular0[] = { 1.0f,1.0f,1.0f,1.0f};
GLfloat specular1[] = { 0.9f,0.9f,0.9f,1.0f};

void redraw(){
    FOR (T,1,model_cnt){
	    FOR (i,0,CSn-1)
		    model[T].cs[i].createSpline(0);
	    model[T].scaleSpline();
	    model[T].quaterSpline();
	    model[T].positionSpline();
	    model[T].crossSectionSpline();
    }
}

void Parsing(char *name){
	char temp[1000];
	FILE *in=fopen(name,"r"), *out=fopen("input_parse.txt","w");
	while (!feof(in)){
		fgets(temp,1000,in);
		for (int i=0;i<strlen(temp);i++){
			if (temp[i]=='#'){ fprintf(out,"\n"); break; }
			fprintf(out,"%c",temp[i]);
		}
	}
	fclose(in), fclose(out);
}

void input(int cnt, char **name){
    model_cnt = cnt-1;
    FOR (T,1,cnt-1){
	    Parsing(name[T]);
	    FILE *in=fopen("input_parse.txt","r");
	    model[T].cs.clear();
	    fscanf(in,"%s",Spline);
	    if (strcmp(Spline,"BSPLINE")==0) spline=BSPLINE;
	    else if (strcmp(Spline,"CATMULL_ROM")==0) spline=CATMULL_ROM;
	    else if (strcmp(Spline,"NATURAL")==0) spline=NATURAL;
	    fscanf(in,"%d",&CSn);
	    fscanf(in,"%d",&N);
	    LOD=lod[lod_idx];	
	    FOR (i,0,CSn-1){
		    model[T].cs.push_back(CS());
		    FOR (j,0,N-1){
			    double x, z;
			    fscanf(in,"%lf %lf",&x,&z);
			    model[T].cs[i].push(x,z);
		    }
		    double angle,x,y,z,scale,tx,ty,tz;
		    fscanf(in,"%lf",&scale);
		    fscanf(in,"%lf %lf %lf %lf",&angle,&x,&y,&z);
		    fscanf(in,"%lf %lf %lf",&tx,&ty,&tz);
		    model[T].pushScale(scale);
		    model[T].pushRotate(angle,x,y,z);
		    model[T].pushPosition(tx,ty,tz);
		    model[T].cs[i].createSpline(0);
	    }
	    model[T].scaleSpline();
	    model[T].quaterSpline();
	    model[T].positionSpline();
	    model[T].crossSectionSpline();
        model[T].setMaterial(default_material);
	    fclose(in);
    }

	BSP(totalPS);
}

void BackwardRayTracing(position P, position v, int depth){
	double s = INF;
	
}

void display() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(fov, (GLdouble)width / (GLdouble)height, Near, Far);
	glOrtho(-300,300,-300,300,0.1,500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadGlobalCoord();


    double camDist = height/2.0 / tan((fov/2.0)*PI/180.0);
	FOR (i,-(height/2),height/2){
		FOR (j,-(width/2),width/2){
			// calc ray direction vector
		    position vec = position((double)i,(double)j,-camDist); vec = vec / norm(vec);
			BackwardRayTracing(eye, vec, 0);
			

		}
	}
    glutSwapBuffers();
}

void Test(){
	BMP test;
	test.SetSize(width,height);
	test.SetBitDepth(32);
	for (int i=0;i<width;i++) for (int j=0;j<height;j++){
		RGBApixel rgba;
		if (100<i && i<150 && 100<j && j<150){
			rgba.Blue = 255;
			rgba.Red = 255;
			rgba.Green = 255;
			rgba.Alpha = (i+j)%256;
		}else{
			rgba.Blue = 0;
			rgba.Red = 0;
			rgba.Green = 0;
			rgba.Alpha = (i+j)%256;
		}

		test.SetPixel(i,j,rgba);
	}
	test.WriteToFile("./test.bmp");
}

int main(int argc, char **argv) {

	Test();

    ManualPrint();
    //input(argc,argv);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width , height);
    glutInitWindowPosition( 50, 50 );
    glutCreateWindow("HW3");
    Init();
	display();
    glutReshapeFunc(resize);
    glutTimerFunc(timeStep, Timer, 0);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    glutMainLoop();
    return 0;
}

void loadGlobalCoord()
{
    glLoadIdentity();
    position eye_new, ori_new, up_new;
    eye_new = calc_rotate(Tot,eye) + translate;
    ori_new = translate;
    up_new = calc_rotate(Tot, position(0,1,0));
    gluLookAt(eye_new.p[0], eye_new.p[1], eye_new.p[2], ori_new.p[0], ori_new.p[1], ori_new.p[2], up_new.p[0], up_new.p[1], up_new.p[2]);

    glMultMatrixd(rotMatrix);
}


//------------------------------------------------------------------------
// Moves the screen based on mouse pressed button
//------------------------------------------------------------------------
position ray(position A, position B, position C, double r){ // A+kB on sphere
    position a = B;
    position b = C-A;
    position p = a * (a%b);
    // a is unit vector
    // p is projection vector of b to a

    double dist_p = norm(p);
    position e = b - p; // p + e = b
    double dist_e = norm(e);
    
    if (dist_e >= 0.9999f * r){
        double max_angle = atan2(r, sqrt(norm(b)*norm(b) - r*r));
        position axis = b * p;
        axis = axis / norm(axis);
        quater Q = quater(cos(max_angle/2), axis.p[0]*sin(max_angle/2), axis.p[1]*sin(max_angle/2), axis.p[2]*sin(max_angle/2));
        p = calc_rotate(Q,b);
        p = p / norm(p) * sqrt(norm(b)*norm(b) - r*r);
    }else{
        p = p / norm(p) * (norm(p) - sqrt(r*r - norm(e)*norm(e)));
    }
    return A+p;
}

void print(position x, string name){
    cout << name;
    fprintf(out," = %.3lf, %.3lf, %.3lf\n",x.p[0],x.p[1],x.p[2]);
}

position D2toD3(int x,int y){ // return position of intercept b/w cam & cos(center of sphere)
    x-=width/2; y=height/2-y;
    double camDist = height/2.0 / tan((fov/2.0)*PI/180.0);
    position vec = position((double)x,(double)y,0.0) - position(0.0,0.0,camDist);
    vec=vec/norm(vec);
    vec = calc_rotate(Tot, vec);

    position eye_new, ori_new; 
    eye_new = calc_rotate(Tot,eye) + translate;
    ori_new = translate;
    return ray(eye_new, vec, ori_new, trackballRadius);
}

void glutMotion(int x, int y)
{
    trackballRadius = eye.p[2] * 0.4 * (fov/45);
    pv=D2toD3(mousePosX, mousePosY);
    v =D2toD3(x,y);
    mousePosX = x;
    mousePosY = y;
    curMouseX = x, curMouseY = y;
    
    double theta = angle(v, pv);
    if (leftButton){
        position cross = pv*v;
        if (norm(cross)<=eps) return;
        cross = cross/norm(cross);
        quater Q = quater(cos(theta/2), cross.p[0]*sin(theta/2), cross.p[1]*sin(theta/2), cross.p[2]*sin(theta/2));
        Q = Q.inverse();
        Tot = Q * Tot;
    }
    return;
}

position getPointOnPlane(int x,int y){
    double proj[16],model[16];
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    double winX,winY;
    GLdouble winZ;
    winX = (double)x;
    winY = height - (double)y;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &winZ);
    winZ/=0.00390619;
    if (winZ>=1-1e-4){ // Pointer doesn't meet object's surface
        return position(9999,9999,9999);
    }
    winZ = 2.0*winZ - 1.0;
    winZ = ((-2.0*Far*Near)/(Far-Near)) / (winZ - (Far + Near) / (Far - Near));
    
    int x2=x-width/2, y2=height/2-y;
    double camDist = height/2.0 / tan((fov/2.0)*PI/180.0);
    position vec = position((double)x2,(double)y2,0.0) - position(0.0,0.0,camDist);
    vec=vec/norm(vec);
    vec = vec * (winZ / abs(vec.p[2]));
    return eye + vec;
}

//------------------------------------------------------------------------
// Function that handles mouse input
//------------------------------------------------------------------------
void glutMouse(int button, int state, int x, int y)
{
    curMouseX = x, curMouseY = y;
    switch ( button )
    {
        case GLUT_LEFT_BUTTON:

            if ( state == GLUT_DOWN )
            {
                mousePosX = x;
                mousePosY = y;
                leftButton = true;
                if (seekFlag == true){
                    seekFlag = false;
                    position newCenter = getPointOnPlane(x,y);
                    if (9999-1e-4<=newCenter.p[0] && newCenter.p[0]<=9999+1e-4){
                    }else{
                        translate = translate + calc_rotate(Tot, (newCenter - ori));
                    }
                } 
            }
            else if ( state == GLUT_UP )
            {
                leftButton = false;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if ( state == GLUT_DOWN ){
                mousePosX = x;
                mousePosY = y;
                rightButton = true;
            }
            else if (state == GLUT_UP){
                rightButton = false;
            }
            break;
        case 3:break;
        default:break;
    }
    return;
}
int TOP=5;
double ANGLE = -30.0;
double dt=0.0f, alpha=0.005;
bool last=false;
typedef double F;
GLdouble vert[]={0,0,0, 10,0,0, 10,10,0, 0,10,0};

void resize(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLdouble)w / (GLdouble)h, .1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        fprintf(out,"Thank you.\n");
        exit(0);
        break;
	case '1':
		swept = (swept + 1) % 3;
		break;
	case '2':
		lod_idx++; if (lod_idx==5) lod_idx=0;
		LOD=lod[lod_idx];
		redraw();
		break;
	case '3':
		lod_idx--; if (lod_idx==-1) lod_idx=4;
		LOD=lod[lod_idx];
		redraw();
		break;
	case '4':
		spline = spline+1;
		if (spline==4) spline=1;
		redraw();
		break;
    case ' ': //////////////////// spacebar for resetting view
        eye=position(0.0f,0.0f,180.0f);
        ori=position(0.0f,0.0f,0.0f);
        rot[0]=0.0f; rot[1]=0.0f; rot[2]=0.0f;
        translate = position(0,0,0);
        fov=45;
        Tot=quater();
        break;
    case 'w': //////////////////// 'w' view up translate
        translate = translate + calc_rotate(Tot, position(0,-0.5,0));
        break;
    case 's': //////////////////// 's' view down translate
        translate = translate + calc_rotate(Tot, position(0,+0.5,0));
        break;
    case 'a': //////////////////// 'a' view left translate
        translate = translate + calc_rotate(Tot, position(+0.5,0,0));
        break;
    case 'd': //////////////////// 'd' view right translate
        translate = translate + calc_rotate(Tot, position(-0.5,0,0));
        break;
    case 'f': //////////////////// 'f' ready for getting mousepoint for 'seek'
        seekFlag = true;
        break;
    case 'b': //////////////////// 'b' move camera backward to show all
        translate = position(0,0,0);
        eye=position(0.0f,0.0f,30.0f / tan((fov/2)*PI/180.0));
        break;
    case 'l': //////////////////// 'l' show Standard Line for check center of rotation
        showStdLine = !showStdLine;
        break;
    case '[': //////////////////// '[' view dolly in
        if (eye.p[2]<=5) break;
        eye.p[2]-=1; break;
    case ']': //////////////////// ']' view dolly out
        if (eye.p[2]>=300) break;
        eye.p[2]+=1; break;
    case ';': //////////////////// ';' view zoom in
        if (fov<=5) break;
        fov -= 1.0; break;
    case '\'': //////////////////// ''' view zoom out
        if (fov>=90) break;
        fov += 1.0; break;
    default:
        break;
    }
}
void Timer(int unused)
{
    //if (dt>0.995f) alpha=-0.005f;
    //if (dt<0.005f) alpha=0.005f; 
    if (dt<=0.995f) dt+=alpha;
    glutPostRedisplay();
    glutTimerFunc(timeStep, Timer, 0);
}

void ManualPrint(){
    fprintf(out,"MANUAL FOR PROGRAM, 2014-16371 Ryu Ho Seok\n");
    fprintf(out,"------------------\n");
    fprintf(out,"[ a ] : move left\n");
    fprintf(out,"[ s ] : move down\n");
    fprintf(out,"[ d ] : move right\n");
    fprintf(out,"[ w ] : move up\n");
    fprintf(out,"------------------\n");
    fprintf(out,"[ [ ] : dolly in,  exists maximum range\n");
    fprintf(out,"[ ] ] : dolly out, exists maximum range\n");
    fprintf(out,"[ ; ] : zoom in,   exists maximum range\n");
    fprintf(out,"[ ' ] : zoom out,  exists maximum range\n");
    fprintf(out,"[ b ] : show all,  if cannot see anything, press 'space bar'\n");
    fprintf(out,"[ f ] : seek, after press f, click mouse on the surface. if not, nothing happens\n");
    fprintf(out,"[ l ] : show or hide standard line for checking the center of rotation\n");
    fprintf(out,"------------------\n");
    fprintf(out,"[ space bar ] : reset fov, cam dist, translate, rotation\n");
    fprintf(out,"[ esc ] : exit program.\n");
}

void Init(){

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_BUFFER_BIT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable( GL_LIGHTING );
}

void GiveMaterial(Material mat){
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &mat.shininess);
    //glColor3fv(mat.diffuse);
}
