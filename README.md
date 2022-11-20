# Lamp Lighter

Allocate lamps along edges to fuel sources at the verices, minimizing the number and reach of the fuel sources reuired.

https://stackoverflow.com/q/74435920/16582

Algorithm pseudocode:

 - WHILE lamps remain unfueled
    - LOOP over sources
       - IF source has exactly one edge with unfueled lamps
           - SET fuel source to source at other end of unfueled edge
           - INCREMENT radius of fuel source with unfueled edge lamp count
           - LOOP over edges on fuel source
               - IF edge fueled lamp count from fuel sourve < fuel source radius
                   - SET edge fueled lamp count from fuel source to fuel source radius 
