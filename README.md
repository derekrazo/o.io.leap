o.io.leap
=========

o.io.leap is a Max external object for the Leap Motion Controller, which outputs OSC into max/msp/jitter.
    
  Also requires: Max SDK http://cycling74.com/products/sdk/ 
   Leap SDK https://leapmotion.com/developers
  
      Version 0.3
        -IDed Hand and finger data (xyz direction, velocity, finger length / width, tool state)
        -Hand numbered palm and sphere data (ie. hand/1/sphere/radius -- gets sphere radius for a first hand)
        -Hand Id'ed pitch, yaw, roll
        -More intuitive name-space design
        
      To-Do:
        -Add leftmost / rightmost
        -Add mathematical data from leap API
        -Add more leap 8.0 features 
      
