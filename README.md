o.io.leap
=========

o.io.leap is a Max external object for the Leap Motion Controller, which outputs OSC into max/msp/jitter.
    
  Build requires: Max SDK http://cycling74.com/products/sdk/ 
   Leap SDK https://leapmotion.com/developers
    Odot binary - email derekrazo@gmail.com
  
      Version 0.6
        -Added "showme" message support, prints namespace to max window
        -Updated gesture data, including new features of down taps, forward taps, swipes, and circles

      Version 0.5
        -can now access handlist directly or hands by ordinal coordinates (leftmost,rightmost,frontmost,backmost)
        -greatly simplifed code for readability and to make adding new features faster
        -added /distance/from + /distance/to for all fingers and hands
        -added support for touch zones
        -updated namespace

      To-Do:
        -Create new gesture with leap api data
        -Create display for o.io.leap
        -Change project name to o.io.leapmotion
        -Update and build against leap 1.x
   
      
