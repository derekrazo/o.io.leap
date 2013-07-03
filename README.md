o.io.leap
=========

o.io.leap is a Max external object for the Leap Motion Controller, which outputs OSC into max/msp/jitter.
    
  Also requires: Max SDK http://cycling74.com/products/sdk/ 
   Leap SDK https://leapmotion.com/developers
  
      Version 0.2
        -IDed Hand and finger data (xyz direction, velocity, finger length / width, tool state)
        -Hand numbered palm and sphere data (ie. /0/sphereRadius -- gets sphere radius for a first hand)
        
      To-Do:
        -Change hands /0/ and /1/ to leftmost and rightmost, respectively
        -Add more leap 8.0 features 
      
