/**
	@file
	simplemax - a max object shell
	jeremy bernstein - jeremy@bootsquad.com	

	@ingroup	examples	
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

#include "osc.h"
#include "osc_bundle_u.h"
#include "osc_message_u.h"
#include "osc_atom_u.h"
#include "osc_mem.h"
#include "Leap.h"

////////////////////////// object struct
typedef struct _simplemax 
{
	t_object					ob;			// the object itself (must be first)
    void* outlet; //pass outlet to outlet function
} t_simplemax; //actual name, typedef

///////////////////////// function prototypes
//// standard set
void *simplemax_new(t_symbol *s, long argc, t_atom *argv);
void simplemax_free(t_simplemax *x);
void simplemax_assist(t_simplemax *x, void *b, long m, long a, char *s);

//////////////////////// global class pointer variable
void *simplemax_class;
void simplemax_anything(t_simplemax *x,t_symbol* msg,int argc,t_atom* argv);

int main(void)
{	
	// object initialization, OLD STYLE
	// setup((t_messlist **)&simplemax_class, (method)simplemax_new, (method)simplemax_free, (short)sizeof(t_simplemax), 
	//		0L, A_GIMME, 0);
    // addmess((method)simplemax_assist,			"assist",		A_CANT, 0);  
	
	// object initialization, NEW STYLE
	t_class *c;
	
	c = class_new("simplemax", (method)simplemax_new, (method)simplemax_free, (long)sizeof(t_simplemax), 
				  0L /* leave NULL!! */, A_GIMME, 0);//if object takes no args, A_GIMME wouldbe removed, A_GIMME - an array of atoms
	
	/* you CAN'T call this from the patcher */
    class_addmethod(c, (method)simplemax_assist,			"assist",		A_CANT, 0);  
	
	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    class_addmethod(c, (method)simplemax_anything, "anything",A_GIMME,0);//(method)simplemax is the method that we bind const char *name to
	simplemax_class = c;

	post("I am the simplemax object");
	return 0;
}

void simplemax_assist(t_simplemax *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "I am inlet %ld", a);
	} 
	else {	// outlet
		sprintf(s, "I am outlet %ld", a); 			
	}
}
        
}

void simplemax_free(t_simplemax *x)
{
	;
}

/*
 A_GIMME signature =
	t_symbol	*s		objectname
	long		argc	num additonal args
	t_atom		*argv	array of t_atom structs
		 type = argv->a_type
		 if (type == A_LONG) ;
		 else if (type == A_FLOAT) ;
		 else if (type == A_SYM) ;
*/
/*
	t_symbol {
		char *s_name;
		t_object *s_thing;
	}
*/
void *simplemax_new(t_symbol *s, long argc, t_atom *argv) //instance constuctor must match A_GIMME ... gensym()... //this is instantiation
{
	t_simplemax *x = NULL;
    long i;
    
	// object instantiation, OLD STYLE
	// if (x = (t_simplemax *)newobject(simplemax_class)) {
	// 	;
	// }
	
	// object instantiation, NEW STYLE
	if (x = (t_simplemax *)object_alloc(simplemax_class)) {
        x->outlet=outlet_new(x, NULL); //second arg is null unless you want to restrict connections
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        /*
        object_post((t_object *)x, "a new %s object was instantiated: 0x%X", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        for (i = 0; i < argc; i++) {
            if ((argv + i)->a_type == A_LONG) {
                object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv+i));
            } else if ((argv + i)->a_type == A_FLOAT) {
                object_post((t_object *)x, "arg %ld: float (%f)", i, atom_getfloat(argv+i));
            } else if ((argv + i)->a_type == A_SYM) {
                object_post((t_object *)x, "arg %ld: symbol (%s)", i, atom_getsym(argv+i)->s_name);
            } else {
                object_error((t_object *)x, "forbidden argument");
            }
        }
    */
        
        
	}
	return (x);
}
