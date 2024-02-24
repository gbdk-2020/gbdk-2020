#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include <stdint.h>
#include "common.h"
#include "PlayerCharacter.h"
#include "camera.h"
#include "level.h"

#define GRAVTY 15
#define GROUND_FRICTION 15
#define PLAYER_CHARACTER_INCREASE_JUMP_TIMER_MAX 20
#define PLAYER_CHARACTER_JUMP_VELOCITY 450
#define PLAYER_CHARACTER_WALK_VELOCITY 225
#define PLAYER_CHARACTER_RUN_VELOCITY 325
#define PLAYER_CHARACTER_WALK_TWO_FRAME_COUNTER 3
#define PLAYER_CHARACTER_RUN_TWO_FRAME_COUNTER 5

#define PLAYER_CHARACTER_METASPRITE_PIVOT_X 12
#define PLAYER_CHARACTER_METASPRITE_PIVOT_Y 6
#define PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH 5
#define PLAYER_CHARACTER_BOUNDING_BOX_HALF_HEIGHT 12
#define PLAYER_CHARACTER_BOUNDING_BOX_HEIGHT 24

uint8_t facingRight =TRUE;

uint8_t playerJumpIncrease = 0;
uint8_t threeFrameCounter=0;
uint16_t playerX, playerY;
int16_t playerXVelocity, playerYVelocity;

void SetupPlayer(){

    // Player will start at 40,40
    // the playerX and playerY variables are scaled, so we shift to the left by 4
    playerX=40<<4;
    playerY=40<<4;

    playerXVelocity=0;
    playerYVelocity=0;

    // Set the DMG color palette that fits PlayerCharacter's spriteset
    OBP0_REG = 0x4E;

    // Put the PlayerCharacter's tiles in VRAM
    set_sprite_data(0,PlayerCharacter_TILE_COUNT,PlayerCharacter_tiles);
}
void UpdatePlayer(){
    
    // Use the run velocity if the B button is held
    // Animate the threeFrameCounter faster when B is held
    int16_t moveSpeed = (joypadCurrent & J_B) ?PLAYER_CHARACTER_RUN_VELOCITY:PLAYER_CHARACTER_WALK_VELOCITY;
    uint8_t threeFrameCounterSpeed = (joypadCurrent & J_B) ? PLAYER_CHARACTER_RUN_TWO_FRAME_COUNTER : PLAYER_CHARACTER_WALK_TWO_FRAME_COUNTER;

    threeFrameCounter+=threeFrameCounterSpeed;
    uint8_t threeFrameCounterValue = threeFrameCounter>>4;
    if(threeFrameCounterValue>=3){
        threeFrameCounter=0;
        threeFrameCounterValue=0;
    }

    uint8_t turning = FALSE;

    if(joypadCurrent &J_RIGHT){

        // If we are facing the other direction?
        if(playerXVelocity<0){

            // Just decrease the velocity, and save that we are turning
            playerXVelocity+=GROUND_FRICTION;
            turning=TRUE;
        }else{
            playerXVelocity=moveSpeed;
            facingRight=TRUE;
        }
    }else if(joypadCurrent &J_LEFT){

        // If we are facing the other direction?
        if(playerXVelocity>0){

            // Just decrease the velocity, and save that we are turning
            playerXVelocity-=GROUND_FRICTION;
            turning=TRUE;
        }else{
            playerXVelocity=-moveSpeed;
            facingRight=FALSE;
        }
        
    }else{

        // Move the x velocity towards 0
        if(playerXVelocity>=GROUND_FRICTION)playerXVelocity-=GROUND_FRICTION;
        else if(playerXVelocity<=GROUND_FRICTION)playerXVelocity+=GROUND_FRICTION;
        else playerXVelocity=0;
    }

    uint16_t playerRealX = playerX>>4;
    uint16_t playerRealY = playerY>>4;

    uint8_t grounded = FALSE;

    // Prevent getting stuck in the ground
    while(IsTileSolid(playerRealX,playerRealY+PLAYER_CHARACTER_BOUNDING_BOX_HEIGHT-1)){
        playerY--;
        playerRealY = playerY>>4;
    }

    /**
     * @brief Important Note: We need can't use the same x for horizontal collision as we do vertical.
     * For the horizontal collisions, we'll use player x + or - 5. 
     * For the vertical collisions, we'll use player x + or - 3.
     * If we use the same offset value for both, you'll see player getting stuck in the ground. 
     * This would be because both are firing & succeeding, and as a result: player's
     * x and y velocities are constantly being set to 0.
     */

    // If the player is moving horizontally
    if(playerXVelocity!=0){

        // If the player is moving to the right
        if(playerXVelocity>0){

            // The player sprite is sort of tall, we need to check in multiple places along the right edge
            // Subtract a little from the top & bottom edges so player doesn't get caught in ceilings/floors
            if(IsTileSolid(playerRealX+PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH,playerRealY+2)||IsTileSolid(playerRealX+PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH,playerRealY+PLAYER_CHARACTER_BOUNDING_BOX_HALF_HEIGHT)||IsTileSolid(playerRealX+PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH,playerRealY+(PLAYER_CHARACTER_BOUNDING_BOX_HEIGHT-2)))playerXVelocity=0;

        // If the player is moving to the left
        }else if(playerXVelocity<0){

            // The player sprite is sort of tall, we need to check in multiple places along the left edge
            // Subtract a little from the top & bottom edges so player doesn't get caught in ceilings/floors
            if(IsTileSolid(playerRealX-PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH,playerRealY+2)||IsTileSolid(playerRealX-PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH,playerRealY+PLAYER_CHARACTER_BOUNDING_BOX_HALF_HEIGHT)||IsTileSolid(playerRealX-PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH,playerRealY+(PLAYER_CHARACTER_BOUNDING_BOX_HEIGHT-2)))playerXVelocity=0;
        }
    }

    // If the player is moving downwards or still
    if(playerYVelocity>=0){

        // Check both sides of player's feet (left and right)
        // Subtract a little from the edge so player doesn't get caught in walls
        if(IsTileSolid(playerRealX+(PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH-2),playerRealY+PLAYER_CHARACTER_BOUNDING_BOX_HEIGHT)||IsTileSolid(playerRealX-(PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH-2),playerRealY+PLAYER_CHARACTER_BOUNDING_BOX_HEIGHT)){
            playerYVelocity=0;
            grounded=TRUE;
        }

    // If the player is moving upwards
    }else if(playerYVelocity<0){
        
            // Check both sides of player's head (left and right)
            // Subtract a little from the edge so player doesn't get caught in walls
            if(IsTileSolid(playerRealX+(PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH-2),playerRealY)||IsTileSolid(playerRealX-(PLAYER_CHARACTER_BOUNDING_BOX_HALF_WIDTH-2),playerRealY)){
            playerYVelocity=0;
        }
    }

    uint8_t pressedA = (joypadCurrent & J_A && !(joypadPrevious & J_A));
    uint8_t pressedUp = (joypadCurrent & J_UP && !(joypadPrevious & J_UP));
    uint8_t pressedAOrUp = pressedA||pressedUp;

    // If we are grounded, and the A/Up button was JUST pressed
    if(grounded && pressedAOrUp){
        playerYVelocity=-PLAYER_CHARACTER_JUMP_VELOCITY;
        playerJumpIncrease=PLAYER_CHARACTER_INCREASE_JUMP_TIMER_MAX;
    }

    // If the player is in the air
    if(!grounded){

        // If we are in the air, increase the amount of time the player can increase the jump height
        if(playerJumpIncrease>0)playerJumpIncrease--;

        // If we are not holding A/Up, or if the amount of time we can increase our jump has ended
        if(!((joypadCurrent & J_A||joypadCurrent & J_UP))||playerJumpIncrease==0){

            // Apply gravity
            playerYVelocity+=GRAVTY;

            // Reset to zero here, so the player has to hold initally to increase jump height
            playerJumpIncrease=0;
        }

    // If the player is grounded, and moving downward
    }else if(playerYVelocity>=0){

        //  Stop the velocity now
        playerYVelocity=0;
    }

    // Apply the players velocity
    playerX+=playerXVelocity>>4;
    playerY+=playerYVelocity>>4;

    // Get the non-scaled version  of the player's position
    playerRealX = playerX>>4;
    playerRealY = playerY>>4;

    // if we've gone past the half-screen mark
    if(playerRealX>=(DEVICE_SCREEN_PX_WIDTH>>1)){
        uint16_t max = currentLevelWidth-DEVICE_SCREEN_PX_WIDTH;
        camera_x=playerRealX-(DEVICE_SCREEN_PX_WIDTH>>1);

        // Limit the camera position to avoid drawing offscreen/looping data
        if(camera_x>max)camera_x=max;
    }
    else camera_x=0;

    // Which of the player's metasprite frames should be shown?
    // If we are grounded:
    //   Turning        = Frame 5
    //   Standing Still = Frame 0
    //   Running        = Frame 0 - 2 (via threeFrameCounterValue variable)
    // If we are in the air:
    //   Rising         = Frame 3
    //   Falling        = Frame 4
    uint8_t frame = grounded ? (turning ? 5 :((playerXVelocity>>4)==0 ? 0 : threeFrameCounterValue)) : (playerYVelocity<0 ? 3 : 4);

    // Get the player's position relative to the camera's position
    uint16_t playerCameraX = (playerRealX-camera_x)+DEVICE_SPRITE_PX_OFFSET_X;
    uint16_t playerCameraY= (playerRealY-camera_y)+DEVICE_SPRITE_PX_OFFSET_Y;

    // Flip horizontally, if we aren't facing right
    if(facingRight)move_metasprite(PlayerCharacter_metasprites[frame],0,0,playerCameraX,playerCameraY);
    else move_metasprite_vflip(PlayerCharacter_metasprites[frame],0,0,playerCameraX,playerCameraY);

    // Increase the level if the player is at the end
    if(playerRealX>currentLevelWidth-32){
        nextLevel++;
    }
}