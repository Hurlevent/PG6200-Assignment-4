# PG6200-innlevering-4

# How to play!

- Pressing the space bar, will make the model rotate.
- Pressing the 'z'-key will make it rotate slower, and pressing the 'x'-key will make it rotate faster
- Pressing '1' makes the program render without using any form of normal mapping technique.
- Pressing '2' makes the program use a normal-map for lightning calculations instead of using normals as vertex attributes. The result will look identical to regular shading
- Pressing '3' makes the program use the normal-map rendering technique with a bump-map to alter the normals used in the lightning calculations to give the illution depth for the surface of the model.

# About the solution

Soo, the first thought you will probably have when looking at the source code, is that there is a lot of "duplicated code". I have tried, and spent a lot of time trying to use the same shader code, and program for both the normal-rendering mode and bump-map-rendering mode. And in theory that should be very easy, you would think that it would be possible to use the normal-map program with a different normal texture would give the same result as the bump-map program. But for some reason, the results was never identical.

Also, it should be noted that the normal program, which uses the source code from "normal.vert" and "normal.frag" is only used in order to make a texture that may be used as a normal map, where all normals point in the same direction. I decided to make the program generate the normal-map for me, so that I didn't have to manually make a bitmap image.
