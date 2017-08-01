#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>
/* Surface to store current scribbles */
static cairo_surface_t *surface = NULL;
static int currenterrorallow = 10;
static int currenterrorallowback = 90;
static int currenterrorallowforward = 10;
static int distmode = 2;//0 = normal, 1 = whatever the complex one 2 in terms of gamma
static int nomonmode = 0;//0 = circlar, 1 = square,2 = for text
static int nextmean = 0;
static int nextsigma = 5;
static int progress = 0;
static int graphdata[100];
static int distribution[100];
static int nextdistribution[100];
static int go = 3;
static int turn = 1;
static int clicked = 0;
static int amountofnomons = 100;
// Nomons go as location x, location y, radius,progress and active
static int nomons[100][5];
//grid system for iagno start with 10x10
static int grid[10][10]; //0 = empty ///1 = white //2 = black
GtkWidget *drawing_area;


static float
powerfunction(float num,float power){
if(power>1){
	int i;
	for(i=0;i<power;i++){
		power*=power;
	}
}
}
static void
clear_surface (void)
{
  
	 cairo_t *cr;

  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 0.8, 0.8, 1);
  cairo_paint (cr);

  cairo_destroy (cr);
}
static char*
getWordOne(int localprogress){
	static char word[10];
		if(localprogress>=40){
			sprintf(word,"Hello");
		}else
		if(localprogress>=30){
			sprintf(word,"Hell");
		}else
		if(localprogress>=20){
			sprintf(word,"Hel");
		}else
		if(localprogress>=10){
			sprintf(word,"He");
		}else
		if(localprogress>=0){
			sprintf(word,"H");
		}
	return word;
}
static char*
getWordTwo(int localprogress){
	static char word[5];
	if(localprogress>=90){
			
			sprintf(word,"World");
		}else
		if(localprogress>=80){
			sprintf(word,"Worl");
		}else
		if(localprogress>=70){
			sprintf(word,"Wor");
		}else
		if(localprogress>=60){
			sprintf(word,"Wo");
		}else
		if(localprogress>=50){
			sprintf(word,"W");
		}else{
			sprintf(word,"");
		}
	return word;
}
static float
convertProgressToAngle(int g){

	float radius = 100.0/(2.0*M_PI);
	float angle = g/radius;
	return angle;
}
static float 
getSumOf(int trig,int current){//0=cos, 1=sin
	float sum = 0;
	for(int z = 0;z<100;z++){
		int amountofpoints = graphdata[z];
		float angle = convertProgressToAngle(z)+convertProgressToAngle(current);
		if(trig==0){
		sum+=cos(angle)*amountofpoints;
		}else{
		sum+=sin(angle)*amountofpoints;
		}
	}
	return sum;
//graphdata[100];
}
static int getTotalDataPoints(){
	int total = 0;
	for(int o = 0; o<100;o++){
		total+=graphdata[o];
	}
	return total;
}
static int 
getProFromProgress(int g){
	//if(distmode==0){
	if(g>50){
		g -= 50;
	}else{
		g += 50;
	}
	//}
	float sigma = nextsigma*2;
	//float distributionvalue = 0;
	float distributionvalue = (900.0/(sigma*pow(2*3.14,0.5))*pow(2.7182,-pow(g-(50+nextmean),2)/(2.0*pow(sigma,2))));
	float angle = convertProgressToAngle(g);
	g_print("Value of progress = %d, Value of angle = %f\n",g,angle);
	float Rzero = 4;
	float cosprior = Rzero*cos(0.1);
	float sinprior = Rzero*sin(0.1);
	float sumofcos = getSumOf(0,g);
	float sumofsin = getSumOf(1,g);
	float rvaluesquared = (pow(cosprior+sumofcos,2)+pow(sinprior+sumofsin,2));
	float rvalue = sqrt(rvaluesquared);
	g_print("r value: %f\n",rvalue);
	float bigN = getTotalDataPoints();
	g_print("Data points: %f\n",bigN);
	float littleu = 5;
	float part1 = log(M_PI*2)*(-(bigN+littleu-1)/2);
	float part2 = log(rvalue)*(-0.5);
	float part3 = log(bigN+littleu-rvalue)*(-(bigN+littleu+1)/2);
	float part4 = lgammaf((bigN+littleu+1)/2);
	g_print("part1 %f part2 %f part3 %f part4 %f\n",part1,part2,part3,part4);
	float evidencelog = part1+part2+part3+part4;
	g_print("evidence log value: %f\n",evidencelog);
	if(distmode == 1){
	
//	
	distributionvalue = -1*evidencelog;	
	//distributionvalue += 200;
//	
/*
	float beta = bigN-1;
	float Izero = 1;
	float mean = convertProgressToAngle(nextmean);
	float priormean = convertProgressToAngle(4);
	float part5 = beta*Rzero*cos(convertProgressToAngle(g)-mean);
	float part6 = beta*Rzero*cos(mean-priormean);
	float part7 = log(2*M_PI*Izero*beta)*(bigN+littleu);
	
	g_print("part5 %f part6 %f part7 %f evidencelog %f\n",part5,part6,part7,evidencelog);
	float logdist = -evidencelog+(part5)+part6-part7;
	g_print("log dist value: %f\n",logdist);
	float dist = exp(logdist);
	g_print("dist value: %f\n",dist);
	*/
	//distributionvalue = 100*dist;
	}else if(distmode==2){
		float part11 = -evidencelog;
		float part12 = (-bigN+littleu)*log(2*M_PI);
		float mean = convertProgressToAngle(nextmean);
		float sig = convertProgressToAngle(nextsigma);
		float part13 = log(bigN+littleu-rvalue*cos(mean-sig))*(-(bigN+littleu+2)/2);
		float part14 = lgammaf((bigN+littleu+2)/2);
		float distributionvaluelog = part11+part12+part13+part14;
		g_print("part11 %f part12 %f part13 %f part14 %f\n",part11,part12,part13,part14);
		g_print("Dist Value Log%f\n",distributionvaluelog);
		distributionvalue = exp(distributionvaluelog/100)*100;
		g_print("Dist Value %f\n",distributionvalue);
	}


	int currentvalue = (int)distributionvalue;
	if(currentvalue>100){currentvalue=100;}
	if(currenterrorallowforward>50 || currenterrorallowback<50){
		//THIS MEANS WE CAN DO A PROPER RANGE
		return currentvalue;
	}else{
		//THIS MEANS WE ARE ACROSS THE TOP OF THE CLOCK
		return currentvalue;
	}
	return 0;

}
static int
getPro (int a){
	int g = nomons[a][3];
	
	return getProFromProgress(g);
	
}
/* Create a new surface of the appropriate size to store our scribbles */
static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event,
                    gpointer           data)
{
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));

  /* Initialize the surface to white */
  clear_surface ();

  /* We've handled the configure event, no need for further processing. */
  
return TRUE;
}

/* Redraw the screen from the surface. Note that the ::draw
 * signal receives a ready-to-be-used cairo_t that is already
 * clipped to only draw the exposed areas of the widget
 */
static gboolean
draw_cb (GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return FALSE;
}
static int 
absolute(int input){
	if(input<0){
	return input*-1;
	}else{
	return input;
	}
}
static int
drawnomon(int x,int y, int radius, int localprogress){

	cairo_t *cr;
	cr = cairo_create (surface);
	//char currentrange[40];
	//	sprintf(currentrange,"Currently testing with %d range",currenterrorallow);
	//	cairo_move_to(cr,x,y+50);
	//	cairo_show_text(cr,currentrange);
	
	
	if(clicked == 0){
		if(localprogress==100){
		localprogress = 1;
		}else{
		localprogress++;
		}
	}

	
      	gtk_widget_queue_draw (drawing_area);
	int errorresult = localprogress;
		if(localprogress>50){
			errorresult = 100-localprogress;
		}

	float asfloat = localprogress;
	float todecimal = (50+abs(asfloat-50))/100.0;
	cairo_set_source_rgb (cr, 0, todecimal, 0);
	if(clicked == 1){
  		cairo_set_source_rgb (cr, 0, 0, 1);
	}
	if(nomonmode ==0){	
	float angle = localprogress*2*3.14/100;
	//g_print("Current angle %f\n",angle);
	float startangle = angle-0.1;
	startangle =-(3.14/2);
	angle-=(3.14/2);
	cairo_move_to(cr,x,y);
	cairo_arc (cr, x, y,radius,startangle,angle);
	
	}else if(nomonmode == 1){
	int height = ((float)(radius*2))*((float)localprogress)/(100.0);
	cairo_rectangle(cr,x-radius,y-radius,radius*2,height);
	}else if(nomonmode == 2){
		char progresstext[40];
		cairo_set_font_size(cr,18);
		
		sprintf(progresstext,"%s",getWordOne(localprogress));
		cairo_move_to(cr,x-radius,y-radius+20);
		cairo_show_text(cr,progresstext);
		cairo_fill (cr);
		sprintf(progresstext,"%s",getWordTwo(localprogress));
		cairo_move_to(cr,x-radius,y-radius+40);
		cairo_show_text(cr,progresstext);
		cairo_fill (cr);
	}
	cairo_fill (cr);

	//
	//cairo_rectangle (cr, 0,0,400,400);
	int u = 0;
	/*for(u=0;u<100;u++){
		int pro = getProFromProgress(u);
		if(pro>0){
		float thisangle = u*2*3.14/100;
		float intensity = ((float)pro)/100.0;
  		cairo_set_source_rgba (cr, 1, 0, 0,intensity);
		cairo_move_to(cr,x,y);
		cairo_arc (cr, x, y,radius,thisangle-(3.14/2)-(3.14/8),thisangle-(3.14/2));
		cairo_fill (cr);
		}
	}*/



  		cairo_set_source_rgb (cr, 1, 0, 1);
		char progressstring[40];
		sprintf(progressstring,"Progress %d%%",localprogress);
		cairo_move_to(cr,x-5,y+25);
		//cairo_show_text(cr,progressstring);
	if(clicked == 1){

  		cairo_set_source_rgb (cr, 1, 0, 1);
		
		
		char errorcalc[40];
		sprintf(errorcalc,"Error = %d",errorresult);
		cairo_move_to(cr,x,y+50);
		cairo_show_text(cr,errorcalc);

		cairo_move_to(cr,x,y+100);
		cairo_show_text(cr,"Is this your final selection? (left click confirm, right click undo)");
	}
	cairo_fill(cr);
	return localprogress;
}


static gboolean
hubloop(){
	//g_print("running\n");

	//rendering section
	cairo_t *cr;
	cr = cairo_create (surface);

	clear_surface();

	//draw the base grid
	int a;int b;
	for(a=0;a<10;a++){
		for(b=0;b<10;b++){
			cairo_set_line_width(cr, 2);  
			cairo_set_source_rgb(cr, 0.69, 0.19, 0);
			cairo_rectangle(cr,a*50,b*50,50,50);
			cairo_stroke(cr);
			if(grid[a][b]==0){
			
			}else{
			if(grid[a][b]==1){
				//white
				cairo_set_source_rgb(cr, 1, 1, 1);
			}else if(grid[a][b]==2){
				//black
				cairo_set_source_rgb(cr, 0, 0, 0);
			}
			cairo_move_to(cr,25+50*a,25+50*b);
			cairo_arc (cr, 25+50*a,25+50*b,25,0,2*3.14);
			cairo_fill (cr);
				//drawcircle
			}
		}
	}

	
	//draw any nomons that exist
	int i;
	for (i=0;i<amountofnomons;i++){
		if(nomons[i][4] == 1){
		nomons[i][3] = drawnomon(nomons[i][0],nomons[i][1],nomons[i][2],nomons[i][3]);
		char pro[40];
		sprintf(pro," %d%%",getPro(i));
		cairo_set_source_rgb(cr, 0,0, 0);
		cairo_move_to(cr,nomons[i][0]-2,nomons[i][1]-2);
		cairo_show_text(cr,pro);
		cairo_fill(cr);
		}
	}
	//DRAW GRAPH
	cairo_set_source_rgb(cr, 0,0.5, 0);

	char errorrangeprint[40];
	sprintf(errorrangeprint,"Current Allowed Range %d%% to %d%%",currenterrorallowback,currenterrorallowforward);
	cairo_move_to(cr,800,25);
	cairo_show_text(cr,errorrangeprint);
cairo_fill(cr);

	cairo_set_source_rgb(cr, 1,0.9, 0.8);
	int toprange = nextmean+nextsigma;
	if(toprange>100){
		toprange-=100;
	}
	int bottomrange = nextmean-nextsigma;
	if(bottomrange<0){
		bottomrange+=100;
	}
	sprintf(errorrangeprint,"New range to try %d%% to %d%% with mean %d, sigma = %d",	bottomrange,toprange,nextmean,nextsigma);
	cairo_move_to(cr,800,35);
	cairo_show_text(cr,errorrangeprint);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 1,1, 1);
	cairo_rectangle(cr,550,50,600,400);
	cairo_fill(cr);	



	
	int graphmaximum = 100;
	int graphheight = 400;
	int graphwidth = 600;
	int graphvalues = 100;
	int barwidth = graphwidth/graphvalues;
	int g;int currentvalue;
	int factor = 900.0;
	int maxvalue = 0;
 	float sigma = nextsigma*2;
	for(g=0;g<100;g++){
		float distributionvalue = (factor/(sigma*pow(2*3.14,0.5))*pow(2.7182,-pow(g-(50+nextmean),2)/(2.0*pow(sigma,2))));
		if(distributionvalue>maxvalue){
			maxvalue = distributionvalue;
		}
	}
		//factor = factor*200/maxvalue;
	for(g=0;g<100;g++){

cairo_set_source_rgba(cr, 0,1, 1,0.5);
		sigma = 5;
		float distributionvalue = (900.0/(sigma*pow(2*3.14,0.5))*pow(2.7182,-pow(g-50,2)/(2.0*pow(sigma,2))));
		currentvalue = (int)distributionvalue;
		int barheight = (currentvalue*graphheight)/graphmaximum;
		cairo_rectangle(cr,550+g*barwidth,450-	barheight,barwidth,barheight);
		//g_print("Value at %d is %f (We approx it to %d,meaning a bar height of %d)",g,distributionvalue,currentvalue,barheight);
		cairo_fill(cr);

cairo_set_source_rgba(cr, 0,1,0,0.5);
		sigma = nextsigma;
		int num = g-50;
		if(num<0){num*=-1;}
		distributionvalue = getProFromProgress(num);
//(factor/(sigma*pow(2*3.14,0.5))*pow(2.7182,-pow(g-(50+nextmean),2)/(2.0*pow(sigma,2))));
		currentvalue = (int)distributionvalue;
		 barheight = (currentvalue*graphheight)/graphmaximum;
		cairo_rectangle(cr,550+g*barwidth,450-	barheight,barwidth,barheight);
		//g_print("Value at %d is %f (We approx it to %d,meaning a bar height of %d)",g,distributionvalue,currentvalue,barheight);
		cairo_fill(cr);
	}
cairo_set_source_rgb(cr, 1, 0, 0);
	for(g=0;g<50;g++){
		currentvalue = graphdata[g];
		int barheight = (currentvalue*graphheight)/graphmaximum;
		cairo_rectangle(cr,550+graphwidth/2+g*barwidth,450-barheight,barwidth,barheight);
		cairo_move_to(cr,550+graphwidth/2+g*barwidth,460);
		if(g%5 == 0){
		char toprint[40];
		sprintf(toprint,"%d",g);
		cairo_show_text(cr,toprint);
		cairo_fill(cr);
		}
	}
	for(g=50;g<100;g++){
		currentvalue = graphdata[g];
		int barheight = (currentvalue*graphheight)/graphmaximum;
		cairo_rectangle(cr,550-graphwidth/2+g*barwidth,450-barheight,barwidth,barheight);
		cairo_move_to(cr,550-graphwidth/2+g*barwidth,460);
		if(g%5 == 0){
		char toprint[40];
		sprintf(toprint,"%d",g);
		cairo_show_text(cr,toprint);
		
		}
		cairo_fill(cr);
	}
	///COMPUTE PROBABILITIES
	//just calculate simple mean and estimate of sigma
	int total = 0;
	int samplesize = 0;
	for(g=0;g<100;g++){
		int cdata = g;
		if(cdata>50){
			cdata-=100;
			//
		}
		samplesize+=graphdata[g];
		total+=graphdata[g]*cdata;
	}
	
	if(samplesize==0){samplesize=1;}//avoiding that 0 div;
	nextmean = total/samplesize;
	//caluculate variance; (and store the current array in a file)
	int sum = 0;
	FILE *f = fopen("data.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
	for(g=0;g<100;g++){
		sum+=pow(g-nextmean,2)*graphdata[g];
		/* print integers and floats */
		fprintf(f, "%d\n", graphdata[g]);
	}
	//g_print("Sum of X-xbar = %d",sum);
	fclose(f);
		if(samplesize>10){
		nextsigma = (pow(sum/(samplesize-1),0.5))/6;
		//g_print("nextsigma = %d",nextsigma);
		if(nextsigma<1){nextsigma=1;}
		}
	if(samplesize>10){
		//g_print("Using new range+mean\n");
		currenterrorallowback = nextmean-nextsigma;
		currenterrorallowforward = nextmean+nextsigma;
		if(currenterrorallowback<0){
			currenterrorallowback = 100+currenterrorallowback;
		}
		if(currenterrorallowforward>100){
			currenterrorallowforward = currenterrorallowforward-100;
		}
		//g_print("Which is %d to %d\n",currenterrorallowback,currenterrorallowforward);
	}		
	
	cairo_destroy (cr);
	gtk_widget_queue_draw_area (drawing_area, 0, 0, 1200, 500);
	return TRUE;

}

static gboolean
isselected (int a){
	int localprogress = nomons[a][3];
	int h;
	int id = 0;
	int prob = 0;
	for(h=0;h<100;h++){
		int cpro = getPro(h);
		if(cpro>prob){
			prob = cpro;
			id = h;
		}else{
			prob = getPro(id);
		}
	}
	g_print("nomon id %d: prob is %d\n",id,prob);
	if(getPro(a)>0.99*prob){//TODO (FUDGE FACTOR)
		return TRUE;
	}
	return FALSE;
}
/* TOP HAT DISTRIBUTION
static gboolean
isselected (int a){
	int localprogress = nomons[a][3];
	if(currenterrorallowforward>50 || currenterrorallowback<50){
		if(localprogress<currenterrorallowforward && localprogress>currenterrorallowback){
		return TRUE;
	}
	}else{
	if(localprogress<currenterrorallowforward ||localprogress>currenterrorallowback){
		return TRUE;
	}
	}
	return FALSE;
}*/
static gboolean 
checker(int increment,int current,int upper){
	if(increment<0){
		return (current>upper);
	}else{
		return (current<upper);
	}
}
static gboolean
trackroute(int lx,int ly,int ux,int uy,int player,int enemy){
	int incrementx = 1;int incrementy = 1;
	if(ux<lx){
		incrementx = -1;
	}
	if(uy<ly){
		incrementy = -1;
	}
	int trackerx;int trackery;
	int turnedover = 0;
	for(trackerx=lx;checker(incrementx,trackerx,ux);trackerx+=incrementx){
	for(trackery=ly;checker(incrementy,trackery,uy);trackery+=incrementy){
		
		if(grid[trackerx][trackery]==player){
			if(turnedover>0){
			//	g_print("Would finish move at %d,%d\n",trackerx,trackery);
				return TRUE;
			}else{
			//g_print("Didn't manage to turn any over at %d,%d\n",trackerx,trackery);
				return FALSE;
			}			
		}else{
			if(grid[trackerx][trackery]==enemy){
				turnedover++;
			//	g_print("Would turn one over! at %d,%d\n",trackerx,trackery);
			}else{
//g_print("No piece at %d,%d (value= %d)\n",trackerx,trackery,grid[trackerx][trackery]);
				return FALSE;	
			}
		}
	}
	}
}
static gboolean
trackdiagonal(int l,int u,int x,int y,int player,int enemy){
	int tracker;
	int turnedover = 0;
	for(tracker=l;tracker<u;tracker++){
		if(grid[x+tracker][y+tracker]==player){
			if(turnedover>0){
				return TRUE;
			}else{
				return FALSE;
			}			
		}else{
			if(grid[x+tracker][y+tracker]==enemy){
				turnedover++;
			}else{
				return FALSE;	
			}
		}
	}
	}

static gboolean
makemove(int x, int y,int player){
	int enemy = 2;
	if(player == 2){
		enemy = 1;
	}
	//8 directions to check
	//start with right
	gboolean validmove = FALSE;
	gboolean result = trackroute(x+1,y,10,y+1,player,enemy);
	if(result){
		validmove = TRUE;
		//g_print("Valid going right");
	}//right
	result = trackroute(x-1,y,0,y+1,player,enemy);
	if(result){
	validmove = TRUE;
		//g_print("Valid going left");
	}//left
	result = trackroute(x,y+1,x+1,10,player,enemy);
	if(result){	
		validmove = TRUE;
		//g_print("Valid going up");
	}//up
	result = trackroute(x,y-1,x+1,0,player,enemy);
	if(result){
		validmove = TRUE;
		//g_print("Valid going down");
	}//down
	/*
	if(trackdiagonal(10-y,10,x,y,player,enemy)){
		validmove = TRUE;
	}//UPRIGHT
	if(trackdiagonal(0,x,0,y,player,enemy)){
		validmove = TRUE;
	}//UPLEFT
	if(trackdiagonal(x,y,x,10,player,enemy)){
		validmove = TRUE;
	}//DOWNRIGHT
	if(trackdiagonal(x,0,x,y+1,player,enemy)){
		validmove = TRUE;
	}//DOWNLEFT
	*/
return validmove;
}
static void 
reactivate(){
	  int a;int b;int amountofactivenomons = 0;
	for(a=0;a<10;a++){
		for(b=0;b<10;b++){
			nomons[10*a+b][0] = 25+50*a;
			nomons[10*a+b][1] = 25+50*b;
			nomons[10*a+b][2] = 25;
			nomons[10*a+b][3] = 10*a+b;
			if(grid[a][b] == 0){
				if(makemove(a,b,turn)){
					nomons[10*a+b][4] = 1;
					amountofactivenomons++;
				}else{
					nomons[10*a+b][4] = 0;
				}
			}else{
				nomons[10*a+b][4] = 0;
			}
		}
	}
	//g_print("\namount of nomons %d\n",amountofactivenomons);
	if(amountofactivenomons>0){
		int c;int phase = 0;
		for(c=0;c<100;c++){
			if(nomons[c][4] == 1){
				nomons[c][3]=(phase*100)/amountofactivenomons;
				//g_print("Current phase %d,generated progress = %d\n",phase,nomons[c][3]);
				phase++;
			}
		}
	}	
}
static void
draw_clock (GtkWidget *widget,gdouble x, gdouble y)
{

	//g_print("clicked");

	if(clicked==1){
	int a;int b;
	
	for(a=0;a<10;a++){
		for(b=0;b<10;b++){
		if(nomons[10*a+b][4] == 1){	
			grid[a][b] = turn;
			g_print("Clicked %d , %d New value of grid square %d\n",a,b,grid[a][b]);
		int xvalue = nomons[10*a+b][3];
		if(xvalue==100){
			xvalue = 0;
		}
		graphdata[xvalue]++; 
			if(turn == 1){
				turn = 2;
			}else{
				turn = 1;
			}
			
		}
	}
	}
	reactivate();
	clicked = 0;
	go++;
	g_print("current go %d\n",go);
	}else{
	int a;

	//clicked = 1;
	int currentlyenabled = 0;
	for(a=0;a<100;a++){
		if(isselected(a)&&nomons[a][4]==1){
			currentlyenabled++;
		}else{
			nomons[a][4] = 0;
		}
	}
	if(currentlyenabled==1){
		clicked = 1;
	}else{
		if(currentlyenabled == 0){
			reactivate();
			clicked = 0;
		}else{
		g_print("%d left\n",currentlyenabled);
		int counter = 0;
		for(a=0;a<100;a++){
			if(isselected(a)&&nomons[a][4]==1){
				nomons[a][3] = (100*counter)/currentlyenabled;
				counter++;
			}
		}
		}
	}
	}
}

/* Handle button press events by either drawing a rectangle
 * or clearing the surface, depending on which button was pressed.
 * The ::button-press signal handler receives a GdkEventButton
 * struct which contains this information.
 */
static gboolean
button_press_event_cb (GtkWidget      *widget,
                       GdkEventButton *event,
                       gpointer        data)
{
  /* paranoia check, in case we haven't gotten a configure event */
  if (surface == NULL)
    return FALSE;
  if (event->button == GDK_BUTTON_PRIMARY)
    {
      draw_clock (widget, event->x, event->y);
    }
  else if (event->button == GDK_BUTTON_SECONDARY)
    {
      reactivate();
      clicked = 0;
    }

  /* We've handled the event, stop processing */
  return TRUE;
}

/* Handle motion events by continuing to draw if button 1 is
 * still held down. The ::motion-notify signal handler receives
 * a GdkEventMotion struct which contains this information.
 */
static gboolean
motion_notify_event_cb (GtkWidget      *widget,
                        GdkEventMotion *event,
                        gpointer        data)
{
  /* paranoia check, in case we haven't gotten a configure event */
  if (surface == NULL)
    return FALSE;

  if (event->state & GDK_BUTTON1_MASK)
    draw_clock (widget, event->x, event->y);

  /* We've handled it, stop processing */
  return TRUE;
}

static void
close_window (void)
{
  if (surface)
    cairo_surface_destroy (surface);
}
static void
generatedata(){
	float prior = 20;
	int g;
	for(g=0;g<100;g++){
		graphdata[g] = (getProFromProgress(g)/prior);
	}

}
static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *frame;
  grid[4][4] = 1;
  grid[4][5] = 2;
  grid[5][4] = 2;
  grid[5][5] = 1;
	generatedata();
  //load data
	FILE *f = fopen("data.txt", "r");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	  
	}else{
	int z;
	char line[128];
	for(z=0;z<100;z++){
		fgets(line,128,f);
		graphdata[z] = atoi(line);
		//g_print("Read in x:%d y:%d",z,graphdata[z]);
	}

	fclose(f);
	}
	
  reactivate();
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Drawing Area");

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  gtk_container_set_border_width (GTK_CONTAINER (window), 8);

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (window), frame);

  drawing_area = gtk_drawing_area_new ();
  /* set a minimum size */
  gtk_widget_set_size_request (drawing_area, 1200, 500);

  gtk_container_add (GTK_CONTAINER (frame), drawing_area);

  /* Signals used to handle the backing surface */
  g_signal_connect (drawing_area, "draw",
                    G_CALLBACK (draw_cb), NULL);
  g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event_cb), NULL);

  /* Event signals */
  g_signal_connect (drawing_area, "motion-notify-event",
                    G_CALLBACK (motion_notify_event_cb), NULL);
  g_signal_connect (drawing_area, "button-press-event",
                    G_CALLBACK (button_press_event_cb), NULL);

  /* Ask to receive events the drawing area doesn't normally
   * subscribe to. In particular, we need to ask for the
   * button press and motion notify events that want to handle.
   */
  gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK);
	

	 /* setup timeout to run the loop */
	g_timeout_add (10,hubloop,NULL);

  gtk_widget_show_all (window);

}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
