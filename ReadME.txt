Hand-crafted BOID algorithm in C++ using SFML

<Global Variables>
* windowX : Width of window
* windowY : Height of window
* debugDrawNeighbors : Draw neighbor perimeter circle for debugging
* flock : Flock vector
* flockSize : Number of birds
* avgBirdVel : Average of birds velocity (Birds initial velocity is determined randomly based on this value)
* avgBirdSize : Average of birds size (Birds size is determined randomly based on this value)

<Flock Config>
* neighborDistance : Minimum distance birds can neighbor each other without repelling
* alignmentStrength : {0~1} how instant the alignment should be, 1 being instant.
* cohesionStrength : {0~1} how strongly the birds lump together, 1 being strongest.
* separationStrength : {1<=} how strongly the birds avoid each other, 1 being weakest.
* desiredSeparation : Desired minimum distance for birds
* axSeparationForce : Maximum separation force
* maxSpeed : Maximum speed for birds so they don't go haywire
* addNoise : true if adding natural noise to birds movement
* randomness : random movement level (advised to keep under 0.1)
