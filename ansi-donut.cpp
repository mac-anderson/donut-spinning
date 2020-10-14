//Console donut rendering program
//2020/10/02


//... it works Debian 10 I know that

#include<iostream>
#include<cmath>
#include<unistd.h>
#include<string>


                        /*All of this math is derived from parametric equations for a torus of revolution.
                          The axis of revolution is the z axis in this case, and the "screen" is the x,y plane.
                          The "camera" also looks down in the negative z direction, so the most positive z values are prioritized
                          when selecting what to display on the screen.

                          The original parametric equations (no "shape rotations" around Z or Y axes) for the torus used here are:
                                x = cos(u)*(LITTLE_RADIUS * cos(v) + BIG_RADIUS)
                                y = sin(u)*(LITTLE_RADIUS * cos(v) + BIG_RADIUS)
                                z = LITTLE_RADIUS * sin(v)

                          To arrive at the post-"shape rotation" components simply multiply this parameterization by a rotation matrix, or two as 
                          is done in this case (this program rotates the torus around the Y and Z axes)
                          
                          The perspective adjustment scales the x,y values by the corresponding z values, shifting z to be strictly
                          positive (hence PERSPECTIVE_NUMBER should be greater than 1, as it is multiplied by the largest possible z
                          value to obtain PERSPECTIVE).

                          The variable light is the dot product of the surface normal of the torus with the vector (0.4,-1,1).
                          The surface normal of the torus can be somehwat intuitively interpreted as kind of like a sphere (degenerate torus);
                          setting BIG_RADIUS = 0 and LITTLE_RADIUS = 1 in the original parameterization of the torus yields a 
                          parameterization for the surface normal of the torus.
                          The components in the "light" calculation are obviously related to the post-rotation components of the torus.
                          
                          light is used to adjust color intensity (each rgb value lies on the interval [0,1]).
                        */




int main()
{

        //values to play around with.*************************************
        //
        //use these to change the appearance of the output.
        //
        //Colors
        const double COLOR_SCHEME[2][3] =       {{0.9, 0.5, 0.0},{1.0, 0.3, 0.4}};
        //                                                              red velvet?:
                                                                  //{0.6, 0.2 ,0.2 }
                                                                  //{1.0, 1.0 ,0.8 }
        //
        //Frosting works with the height (z coord) [-1,1] of the original torus.
        //-1 - 1, -1 is completely frosted; 0 is top half; 1 is not frosted.
        //It behaves in a sine-like fashion. Test to see.
        const double FROSTING = 0;
        //
        const double BIG_RADIUS =       8;
        const double LITTLE_RADIUS= 4;
        //
        //note that because each 'pixel' actually takes two 
        //characters you may need to double your actual window's width.
        const int FRAME_HEIGHT = 4*BIG_RADIUS;
        const int FRAME_WIDTH = FRAME_HEIGHT;
        //
        const double Y_AXIS_SPEED = 1.8;
        const double Z_AXIS_SPEED = 0.4;
        const double FRAME_LENGTH_SECONDS = 0.05;
        //should be greater than 1, smaller values cause more distortion.
        //I recommend about 2.
        const double PERSPECTIVE_NUMBER = 2;
        //
        //***********************************************************
        //***********************************************************





        const double PERSPECTIVE = PERSPECTIVE_NUMBER*(LITTLE_RADIUS + BIG_RADIUS);
        //the third array in screen should store color information. Not highly utilized... yet?
        int screen[FRAME_HEIGHT][FRAME_WIDTH][2];
        double topzvalue[FRAME_HEIGHT][FRAME_WIDTH];

        double radiansY=0;
        double radiansZ=0;

        double x;
        double y;
        double z;
        double cosu;
        double cosv;
        double sinu;
        double sinv;
        double cosRadY;
        double sinRadY;
        double cosRadZ;
        double sinRadZ;
        double lcb;
        int light;

        double rF;
        double gF;
        double bF;
        double rB;
        double gB;
        double bB;

        int xInt;
        int yInt;

        int fgColor;
        int bgColor;


        while(true)
        {
                //clear screen character to start with a clean "canvas"
                std::cout << "\033[2J"; 



                //initialize screen and z values
                for(int i=0;i<FRAME_HEIGHT;i++)
                {
                        for(int j =0;j<FRAME_WIDTH;j++)
                        {
                                screen[i][j][0] = 0;
                                screen[i][j][1] = 2;
                                topzvalue[i][j] = -999;
                        }
                }


                //parameter loop
                //increments for u/v affect the "resolution" of the surface.
                //dark bands are usually a result of insufficient "resolution".
                for(double u = 0; u <= 6.3; u+=0.02)
                {
                        for(double v = 0; v <= 6.3; v+=0.02)
                        {
                                //computing once
                                cosu = cos(u);
                                cosv = cos(v);
                                sinu = sin(u);
                                sinv = sin(v);
                                cosRadY = cos(radiansY);
                                sinRadY = sin(radiansY);
                                cosRadZ = cos(radiansZ);
                                sinRadZ = sin(radiansZ);
                                lcb = LITTLE_RADIUS*cosv+BIG_RADIUS;


                                //calculating the light value, int [0,255].
                                light = round(((0.4*cosRadY*cosRadZ*cosu*cosv+0.4*sinRadZ*sinu*cosv+sinRadY*cosRadZ*sinv/*<-x */ \
                                +cosRadY*sinRadZ*cosu*cosv-cosRadZ*sinu*cosv+sinRadY*sinRadZ*sinv/*<-y */\
                                +cosRadY*sinv-sinRadY*cosu*cosv)/3.753 + 0.5)*255);/*<-z and interval adjustments*/


                                //Where components "land" after rotation + perspective adjustments for x and y.
                                z = cosRadY*LITTLE_RADIUS*sinv-sinRadY*cosu*lcb;

                                x = FRAME_WIDTH/2 + -PERSPECTIVE*(cosRadY*cosRadZ*cosu*lcb+sinRadZ*sinu*lcb+ \
                                sinRadY*cosRadZ*LITTLE_RADIUS*sinv)/(z-PERSPECTIVE);

                                y = FRAME_HEIGHT/2 + -PERSPECTIVE*(-cosRadY*sinRadZ*cosu*lcb+cosRadZ*sinu*lcb- \
                                sinRadY*sinRadZ*LITTLE_RADIUS*sinv)/(z-PERSPECTIVE);



                                //"calculating" the screen for this frame
                                if(y < FRAME_HEIGHT && y >= 0 && x < FRAME_WIDTH && x >= 0)
                                {

                                        xInt = x;//cast int
                                        yInt = y;

                                        //if an x,y value that the torus maps to has already been mapped, 
                                        //only take the highest z value ('closest') point.
                                        if(topzvalue[yInt][xInt] < z)
                                        {
                                                topzvalue[yInt][xInt] = z;
                                                screen[yInt][xInt][0] = light;
                                                //if (pre-rotation) z value is high enough; frosting.
                                                if(sinv > FROSTING)
                                                {
                                                        screen[yInt][xInt][1] = 1;
                                                }
                                                //just regular donut.
                                                else
                                                {
                                                        screen[yInt][xInt][1] = 0;
                                                }
                                        }
                                }
                        }
                }
                //frame has been generated for every u,v value.


                //now print out the frame to the screen
                for(int i=0;i<FRAME_HEIGHT;i+=2)
                {
                        for(int j =0;j<FRAME_WIDTH;j++)
                        {
                                //idea is to use the half block to effectively double
                                //the resolution, as foreground/background color can be
                                //set independently of each other with ANSI codes.
                                fgColor = screen[i][j][1];
                                bgColor = screen[i+1][j][1];
                                rF = COLOR_SCHEME[fgColor][0];
                                gF = COLOR_SCHEME[fgColor][1];
                                bF = COLOR_SCHEME[fgColor][2];
                                rB = COLOR_SCHEME[bgColor][0];
                                gB = COLOR_SCHEME[bgColor][1];
                                bB = COLOR_SCHEME[bgColor][2];

                                //set colors, print character
                                //background first...
                                std::cout << "\033[48;2;" + std::to_string((int)(screen[i][j][0] * rF)) + ";" + std::to_string((int)(screen[i][j][0] * gF)) + ";" + std::to_string((int)(screen[i][j][0] * bF)) + "m";
                                //foreground next
                                std::cout << "\033[38;2;" + std::to_string((int)(screen[i+1][j][0] * rB)) + ";" + std::to_string((int)(screen[i+1][j][0] * gB)) + ";" + std::to_string((int)(screen[i+1][j][0] * bB)) + "m";
                                //print the actual character (half block)
                                std::cout << "\u2584";
                                //
                        }
                        //row printed, onto the next.
                        std::cout << std::endl;
                }


                //increment rotations
                radiansY+=Y_AXIS_SPEED*FRAME_LENGTH_SECONDS;
                radiansZ+=Z_AXIS_SPEED*FRAME_LENGTH_SECONDS;
                //sleep for frame length
                sleep(FRAME_LENGTH_SECONDS);
                //onto next frame...
        }
        return 0;
}
