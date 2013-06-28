#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

#include "osc.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"
#include "osc_mem.h"

#include "Leap.h"

#include <iostream>

#define _USE_MATH_DEFINES // To get definition of M_PI
#include <math.h>

using namespace std;
#include <string>

////////////////////////// object struct
typedef struct _oleap
{
	t_object	ob;
	int64_t		frame_id_save;
	void		*outlet;
	Leap::Controller	*leap;
} t_oleap;

///////////////////////// function prototypes
//// standard set
void *oleap_new(t_symbol *s, long argc, t_atom *argv);
void oleap_free(t_oleap *x);
void oleap_assist(t_oleap *x, void *b, long m, long a, char *s);

void oleap_bang(t_oleap *x);

//////////////////////// global class pointer variable
void *oleap_class;

//////////////////////// Max functions
int main(void)
{
	t_class *c;
	
	c = class_new("o.io.leap", (method)oleap_new, (method)oleap_free, (long)sizeof(t_oleap),
				  0L /* leave NULL!! */, A_GIMME, 0);
	
    class_addmethod(c, (method)oleap_bang, "bang", 0);
    
	/* you CAN'T call this from the patcher */
    class_addmethod(c, (method)oleap_assist, "assist", A_CANT, 0);
	
	class_register(CLASS_BOX, c);
	oleap_class = c;
    
	return 0;
}

void oleap_assist(t_oleap *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "bang to cause the frame data output");
	}
	else {	// outlet
		sprintf(s, "osc(frame data)");
	}
}

void oleap_free(t_oleap *x)
{
	delete (Leap::Controller *)(x->leap);
}

void oleap_bang(t_oleap *x)
{
	const Leap::Frame frame = x->leap->frame();
	const int64_t frame_id = frame.id();
	
	// ignore the same frame
	if (frame_id == x->frame_id_save) return;
	x->frame_id_save = frame_id;
	
	//outlet_anything(x->outlet, gensym("frame_start"), 0, nil);
    
	const Leap::HandList hands = frame.hands();
	const size_t numHands = hands.count();
    
    t_osc_bundle_u *bundle = osc_bundle_u_alloc();//alloc creates memory for and initializes the bundle
    
    
    
    
    //////////////////////////
    t_osc_message_u *time_stamp = osc_message_u_alloc();
    t_osc_message_u *num_hands = osc_message_u_alloc();
    
    osc_message_u_setAddress(time_stamp, "/time_stamp");
    osc_message_u_setAddress(num_hands, "/hands");
    
    t_osc_atom_u* tstamp = osc_atom_u_alloc();
    t_osc_atom_u* nhands = osc_atom_u_alloc();
    
    osc_atom_u_setInt32(tstamp, frame.timestamp());
    osc_message_u_appendAtom(time_stamp, tstamp);
    
    osc_atom_u_setInt32(nhands, numHands);
    osc_message_u_appendAtom(num_hands, nhands);
    
    osc_bundle_u_addMsg(bundle, time_stamp);
    osc_bundle_u_addMsg(bundle, num_hands);
    
    
    //////////////////////////
    for(size_t i = 0; i < numHands; i++)
	{
		// Hand
		const Leap::Hand &hand = hands[i];
		const int32_t hand_id = hand.id();
		const Leap::FingerList &fingers = hand.fingers();
		const size_t numFingers = fingers.count();
        
        t_osc_message_u *hID = osc_message_u_alloc();
        t_osc_message_u *nFingers = osc_message_u_alloc();
        
        osc_message_u_setAddress(hID, "/handID");
        osc_message_u_setAddress(nFingers, "/fingers");
        
        t_osc_atom_u* hand_ID= osc_atom_u_alloc();
        t_osc_atom_u* n_fingers = osc_atom_u_alloc();
        
        osc_atom_u_setInt32(hand_ID, hand_id);
        osc_message_u_appendAtom(hID, hand_ID);
        
        osc_atom_u_setInt32(n_fingers, numFingers);
        osc_message_u_appendAtom(nFingers, n_fingers);
        
        osc_bundle_u_addMsg(bundle, hID);
        osc_bundle_u_addMsg(bundle, nFingers);
        
        
		for(size_t j = 0; j < numFingers; j++)
		{
			// Finger
			const Leap::Finger &finger = fingers[j];
			const int32_t finger_id = finger.id();
			//const Leap::Ray& tip = finger.tip();
			const Leap::Vector direction = finger.direction();
			const Leap::Vector position = finger.tipPosition();
			const Leap::Vector velocity = finger.tipVelocity();
			const double width = finger.width();
			const double length = finger.length();
			const bool isTool = finger.isTool();
            
            
            /*
            string names [14]= {“xpos”,”ypos”,”zpos”,”xdir”,”ydir”,”zdir”,”xvel”,”yvel”,”zvel,finger_length”,”istool_mes”};
            
            for(LOOP OVER FINGERS “J”)
            {
                t_osc_message_u *handdata[14];
                for(int i=0;i<14;i++)
                {
                    handdata[i]=osc_message_u_alloc();
                    osc_message_u_setAddress(handdata[i], “/”+j.toString()+ ”/” +names[i]);
                }	
                
            }
            */
            
            char buff[128];
            
            t_osc_message_u *finger_id_mes = osc_message_u_alloc();
            t_osc_message_u *hand_id_mes = osc_message_u_alloc();
            t_osc_message_u *xpos= osc_message_u_alloc();
            t_osc_message_u *ypos = osc_message_u_alloc();
            t_osc_message_u *zpos = osc_message_u_alloc();
            t_osc_message_u *xdir = osc_message_u_alloc();
            t_osc_message_u *ydir = osc_message_u_alloc();
            t_osc_message_u *zdir = osc_message_u_alloc();
            t_osc_message_u *xvel = osc_message_u_alloc();
            t_osc_message_u *yvel = osc_message_u_alloc();
            t_osc_message_u *zvel = osc_message_u_alloc();
            t_osc_message_u *finger_width = osc_message_u_alloc();
            t_osc_message_u *finger_length = osc_message_u_alloc();
            t_osc_message_u *istool_mes = osc_message_u_alloc();
            
            sprintf(buff,"/%d/fingerID",j);
            osc_message_u_setAddress(finger_id_mes, buff);
            
            sprintf(buff,"/%d/handId",j);
            osc_message_u_setAddress(hand_id_mes, buff);
            
            sprintf(buff,"/%d/xpos",j);
            osc_message_u_setAddress(xpos, buff);
            
            sprintf(buff,"/%d/ypos",j);
            osc_message_u_setAddress(ypos, buff);
            
            sprintf(buff,"/%d/zpos",j);
            osc_message_u_setAddress(zpos, buff);
            
            sprintf(buff,"/%d/xdir",j);
            osc_message_u_setAddress(xdir, buff);
            
            sprintf(buff,"/%d/ydir",j);
            osc_message_u_setAddress(ydir, buff);
            
            sprintf(buff,"/%d/zdir",j);
            osc_message_u_setAddress(zdir, buff);
            
            sprintf(buff,"/%d/xvel",j);
            osc_message_u_setAddress(xvel, buff);
            
            sprintf(buff,"/%d/yvel",j);
            osc_message_u_setAddress(yvel, buff);
            
            sprintf(buff,"/%d/zvel",j);
            osc_message_u_setAddress(zvel, buff);
            
            sprintf(buff,"/%d/finger_width",j);
            osc_message_u_setAddress(finger_width, buff);
            
            sprintf(buff,"/%d/finger_length",j);
            osc_message_u_setAddress(finger_length, buff);
            
            sprintf(buff,"/%d/is_tool",j);
            osc_message_u_setAddress(istool_mes, buff);
            
            t_osc_atom_u* fid_atom = osc_atom_u_alloc();
            t_osc_atom_u* hid_atom = osc_atom_u_alloc();
            t_osc_atom_u* xpos_atom = osc_atom_u_alloc();
            t_osc_atom_u* ypos_atom = osc_atom_u_alloc();
            t_osc_atom_u* zpos_atom= osc_atom_u_alloc();
            t_osc_atom_u* xdir_atom = osc_atom_u_alloc();
            t_osc_atom_u* ydir_atom = osc_atom_u_alloc();
            t_osc_atom_u* zdir_atom = osc_atom_u_alloc();
            t_osc_atom_u* xvel_atom = osc_atom_u_alloc();
            t_osc_atom_u* yvel_atom = osc_atom_u_alloc();
            t_osc_atom_u* zvel_atom = osc_atom_u_alloc();
            t_osc_atom_u* fwidth_atom = osc_atom_u_alloc();
            t_osc_atom_u* flength_atom= osc_atom_u_alloc();
            t_osc_atom_u* istool_atom = osc_atom_u_alloc();
            
            osc_atom_u_setInt32(fid_atom, finger_id);
            osc_message_u_appendAtom(finger_id_mes, fid_atom);
            
            osc_atom_u_setInt32(hid_atom, hand_id);
            osc_message_u_appendAtom(hand_id_mes, hid_atom);
            
            osc_atom_u_setInt32(xpos_atom, position.x);
            osc_message_u_appendAtom(xpos, xpos_atom);
            
            osc_atom_u_setInt32(ypos_atom, position.y);
            osc_message_u_appendAtom(ypos, ypos_atom);
            
            osc_atom_u_setInt32(zpos_atom, position.z);
            osc_message_u_appendAtom(zpos, zpos_atom);
            
            osc_atom_u_setFloat(xdir_atom, direction.x);
            osc_message_u_appendAtom(xdir, xdir_atom);
            
            osc_atom_u_setFloat(ydir_atom, direction.y);
            osc_message_u_appendAtom(ydir, ydir_atom);
            
            osc_atom_u_setFloat(zdir_atom, direction.z);
            osc_message_u_appendAtom(zdir, zdir_atom);
            
            osc_atom_u_setInt32(xvel_atom, velocity.x);
            osc_message_u_appendAtom(xvel, xvel_atom);
           
            osc_atom_u_setInt32(yvel_atom, velocity.y);
            osc_message_u_appendAtom(yvel, yvel_atom);
            
            osc_atom_u_setInt32(zvel_atom, velocity.z);
            osc_message_u_appendAtom(zvel, zvel_atom);
            
            osc_atom_u_setInt32(fwidth_atom, width);
            osc_message_u_appendAtom(finger_width, fwidth_atom);
            
            osc_atom_u_setInt32(flength_atom, length);
            osc_message_u_appendAtom(finger_length, flength_atom);
     
            osc_atom_u_setInt32(istool_atom, isTool);
            osc_message_u_appendAtom(istool_mes, istool_atom);
            
            osc_bundle_u_addMsg(bundle, finger_id_mes);
            osc_bundle_u_addMsg(bundle, hand_id_mes);
            osc_bundle_u_addMsg(bundle, xpos);
            osc_bundle_u_addMsg(bundle, ypos);
            osc_bundle_u_addMsg(bundle, zpos);
            osc_bundle_u_addMsg(bundle, xdir);
            osc_bundle_u_addMsg(bundle, ydir);
            osc_bundle_u_addMsg(bundle, zdir);
            osc_bundle_u_addMsg(bundle, xvel);
            osc_bundle_u_addMsg(bundle, yvel);
            osc_bundle_u_addMsg(bundle, zvel);
            osc_bundle_u_addMsg(bundle, finger_length);
            osc_bundle_u_addMsg(bundle, finger_width);
            osc_bundle_u_addMsg(bundle, istool_mes);
            
		}


        
        
    }


    
    //osc_message_u_appendFloat(msg, 15.0);
    
    long bytes = 0;//length of byte array
    char* pointer = NULL;
    
    osc_bundle_u_serialize(bundle, &bytes, &pointer);//& is address of the variable
    //post("%ld %p", bytes,pointer);
    
    t_atom out[2];
    atom_setlong(out, bytes);
    atom_setlong(out+1, (long)pointer);
    outlet_anything(x->outlet, gensym("FullPacket"), 2, out);
    
    osc_bundle_u_free(bundle);//get rid of stuff in osc message
    osc_mem_free(pointer);//marks pointer address as being free (clear if you want to keep using same)
    

}

void *oleap_new(t_symbol *s, long argc, t_atom *argv)
{
	t_oleap *x = NULL;
    
	if (x = (t_oleap *)object_alloc((t_class *)oleap_class))
	{
		object_post((t_object *)x, "o.io.leap object for leap 7.0");
        
		x->frame_id_save = 0;
		x->outlet = outlet_new(x, NULL);
		
		// Create a controller 
		x->leap = new Leap::Controller;
	}
	return (x);
}
