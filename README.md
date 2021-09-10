# DungeonGame-CPP

This is very much a passion project of mine (that I sadly haven't gotten much work done on lately). I am a HUGE fan of classic Roguelikes (namely Angband) and I've always wanted to make my own, or at least clone my own. This is my own prototype of an Angband clone, using a custom Template system very reminiscient of how Angband does things.

### Dungeon Generation

I did some reading into how Angband (and Angband variants) handle dungeon generation, but I intentionally didn't look at any external code when doing my own dungeon generation. The way that I do things is that I first pick a number of rooms I would like to generate, though this only works as the maximum number that will generate. Then, for every room I need to generate, I pick a random Room Template Prototype (you can see the room templates in `data/rooms.txt`) and pick a random tile to attempt to start creating the room from. I then check to see if this room overlaps with any previously claimed tiles. If there is an overlap, I retry (up to MAX_ROOM_GEN_TRIES). If retries exceed MAX_ROOM_GEN_TRIES, the room simply does not generate and the process moves to the next room if possible. If there is no overlap, I claim all the tiles and generate the Prototype into a full Room. As I generate these rooms, I keep a list of "connection points" which I use for corridors.

Once all the rooms are generated, I begin trying to connect them with corridors. I start by trying to connect room 0 with rooms 1-n, followed by room 1 with rooms 2-n, etc. To connect rooms, I pick 2 random Connection Points from the list I generated previously and perform a basic walk from the starting connection point to the ending connection point. If the corridor exceeds MAX_CORRIDOR_LENGTH, the corridor fails and is aborted. If the corridor hits a floor tile before it reaches its endpoint, I stop prematurely and finish the corridor. Otherwise, the corridor finishes its random walk completely and the corridor is finished. I then convert all tiles in the random walk to floor tiles if the corridor did not abort.

Lastly, I try and place the Player in the dungeon. As a continuity measure, the player always starts on a stairwell that is in the correct direction (If they player descened a stairwell, they should start the next floor an an ascending stairwell). However, there is no actual guarantee that such a stairwell will generate so I first check that any such stairwell even exists, and then I generate one randomly if needed.

### Everything Else

There really isn't much in this prototype, and it serves more as a tech demo of the rudimentary templating system I was working on. Enemies work on VERY simple AI, and can utilize A* pathfinding (rather inefficiently). There are doors that the player can open, and there is a basic fog-of-war system.

### Controls

You can use the numberpad to move or you can use the arrow keys. To move up a stairwell press `,` and to move down a stairwell press `.`. To exit the game, press `q`. Some small debug features are left in as well that are as follows:

`f`: Generate a new dungeon

`g`: Mark the entire dungeon as "seen"

`h`: Pass the turn

`CLICK`: Try and pathfind the player to the tile under the mouse cursor

## WARNING

This is a barely working prototype/tech demo. There ARE bugs, and this code is relatively inefficient. Compile at your own risk.
