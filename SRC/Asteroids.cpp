#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "Explosion.h"
#include "BonusLife.h"
#include <cmath>
#include <math.h>
#include <iostream>

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = -1;
	demoMode = true;
	mAsteroidCount = 10;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);


	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);
	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);
	//Adds this class as a listener
	mAIController.AddListener(thisPtr);
	
	//Adds listener for AIControls to the world
	mGameWorld->AddListener(&mAIController);
	
	//Creates the GUI
	CreateGUI();

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);
	Animation* explosion_anim
		= AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024,
			64, 64, "explosion_fs.png");
	Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1",
		128, 8192, 128, 128, "asteroid1_fs.png");
	Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship",
		128, 128, 128, 128, "spaceship_fs.png");

	Animation* BonusLife = AnimationManager::GetInstance().CreateAnimationFromFile("bonusLife",
		313, 269, 313, 269, "bonusLife.png");
	
	
	// Create a spaceship and add it to the world
	mGameWorld->AddObject(CreateSpaceship());
	// Create mAsteroid count number of asteroids and add them to the world
	mAsteroidCount = 10;
	CreateAsteroids(mAsteroidCount);

	
	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	//If the game is in demo mode
	if (demoMode == true) {
		switch (key) {
		//If enter key is pressed
		case 13:
			//Clear the demo world and begin playable game
			SetTimer(500, CLEAR_DEMO);
			break;
		}
	}
	//If game is not in demo mode
	else {
		switch (key)
		{
			//On Space key pressed
		case ' ':
			//Trigger spaceship to Shoot
			mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
	
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	//If the game is not in demo mode
	//Done so user cannot control the game when it is in demo mode
	if (demoMode == false) {
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
			// Default case - do nothing
		default: break;
		}
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
		// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
		// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
		// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}

//Simulates key events to allow the AI spaceship to move in demo mode
void Asteroids::OnAIMovementPressed(int key, int x, int y)
{
	switch (key)
	{
		// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
		// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
		// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
		// Default case - do nothing
	default: break;
	}

}

void Asteroids::OnAIMovementReleased(int key, int x, int y)
{
	switch (key)
	{
		// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
		// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
		// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
		// Default case - do nothing
	default: break;
	}
}

//If the score changes update score counter
void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

//AI Detects position of the closest asteroids to it's position
void Asteroids::AIDetectAsteroids(bool detect) {
	//Current spaceship position
	GLVector3f spaceShipPosition = mSpaceship.get()->GetPosition();
	//List of GameObjects present in the world
	GameObjectList gameObjectList = mGameWorld->GetGameObjects();
	//Sets the closest distance to the largest possible value for it to be updated by actual asteroids
	closestTargetDistance = mGameWorld->GetWidth();

	//For all objects in the GameWorld
	for (GameObjectList::iterator it = gameObjectList.begin(); it != gameObjectList.end(); ++it) {
		shared_ptr<GameObject> thisPtr = (*it)->GetThisPtr();
		//If the object detected is of type Asteroid
		if (thisPtr.get()->GetType().GetTypeName() == "Asteroid") {
			//Get the asteroids current position
			GLVector3f asteroidPosition = thisPtr.get()->GetPosition();
			//Calculate the difference in x and y positions between the asteroid and spaceship
			float dx = abs(spaceShipPosition.x - asteroidPosition.x);
			float dy = abs(spaceShipPosition.y - asteroidPosition.y);
			//Calculate the actual distance between the asteroid and the spaceship using pythagoras theorem
			float distance = sqrt(pow(dx, 2) + pow(dy, 2));
			//If the distance of the current asteroid is less than the distance of the previous closest asteroid
			if (distance < closestTargetDistance) {
				//Sets the closest target and closest target distance to the current asteroid and its ditance
				closestTarget = thisPtr;
				closestTargetDistance = distance;
			}
		}
	}
	//If the closest target has changed
	if (previousClosestTarget != closestTarget) {
		//Update the closest target
		previousClosestTarget = closestTarget;
		//Stop all rotation movement for AI to then adjust to the main target
		OnAIMovementReleased(100, 304, 77);
		OnAIMovementReleased(102, 304, 77);
	}

	previousClosestDistance = closestTargetDistance;

	//Now that closest target has been found Ai must decide what to do
	AIController(closestTarget);
}

//Controls the AI's movement and shooting
void Asteroids::AIController(shared_ptr<GameObject> closestTarget){
	//Get the spaceship's current position
	GLVector3f spaceShipPosition = mSpaceship.get()->GetPosition();
	//Assign the x and y position of the closest target to variables
	float closestx = closestTarget.get()->GetPosition().x;
	float closesty = closestTarget.get()->GetPosition().y;
	//Calculate the difference between the ship and asteroids x and y values
	float dx = abs(spaceShipPosition.x - closestx);
	float dy = abs(spaceShipPosition.y - closesty);
	//Using the diffence of x and y values calculate the actual distance between the spaceship and the target
	//Does this through pythagoras
	float c  = sqrt(pow(dx, 2) + pow(dy, 2));

	//If the ship is currently reloading decrease the reload timer by 1
	if (reloadTimer != 0) { reloadTimer--; }
	//Compass directions for the ship to face
	string direction1;
	string direction2;

	//Equation to convert from radians to degrees
	float rad2Deg = 180.0 / M_PI;
	//Assign a deadzone degree for the AI to have some aim assist
	float deadzone = 15;
	//Calculates the angle the asteroid is at from the ship using arcsin equation.
	//Used Sin instead of tan as tan has asymptotes at certain degrees
	float targetDegree = asin(dy / c) * rad2Deg;
	//Retrieves the ships current angle
	float currentAngle = mSpaceship.get()->GetAngle();

	//If the asteroids y position is above the spaceship set direction1 to North
	if (closesty >= spaceShipPosition.y){direction1 = "North";}
	//Else set direction to south
	else { direction1 = "South"; }

	//If asteroid x position is greater than the ship's x position set direction2 to east
	if (closestx >= spaceShipPosition.x) {direction2 = "East";}
	//Else set direction2 to West
	else { direction2 = "West"; }
	

	OnAIMovementReleased(101, 257, 269);
	
	//Target is North
	if (direction1 == "North") {
		//North East Target
		if (direction2 == "East") {
			float rightDeadzone = targetDegree - deadzone;
			if (rightDeadzone < 0) { rightDeadzone = 360 - rightDeadzone;}
			//If ship is facing the target
			if (((targetDegree + deadzone) >= currentAngle) && ((rightDeadzone) <= currentAngle)) {
				//Make AI decision
				AIDecision(c);
			}
			//If ship is facing too far left
			else if (targetDegree < currentAngle){
				//Rotate Right
				OnAIMovementPressed(102, 304, 77);
				
			}
			//If ship is facing too far right
			else if (targetDegree > currentAngle) {
				//Rotate Left
				OnAIMovementPressed(100, 304, 77);
			}
		}
		//North West Target
		else {
			//If ship is facing target
			if (((180 - targetDegree - deadzone) <= currentAngle) && ((180 - targetDegree + deadzone) >= currentAngle)) {
				//Make AI Decision
				AIDecision(c);
			}
			//If ship is facing too far left
			else if (180 - targetDegree < currentAngle) {
				//Rotate Right
				OnAIMovementPressed(102, 304, 77);
				
			}
			//If ship is facing too far right
			else if (180 - targetDegree > currentAngle){
				//Rotate Left
				OnAIMovementPressed(100, 304, 77);
				
			}
		}
	}
	//South Target
	else if (direction1 == "South") {
		//South East Target
		if (direction2 == "East") {
			//If ship is facing target
			if ((360 - targetDegree - deadzone <= currentAngle) && ((360 - targetDegree + deadzone >= currentAngle) || (360 - targetDegree + deadzone - 360))) {
				//Make AI Decision
				AIDecision(c);
				
			}
			//If ship is facing too far left
			else if ((360 - targetDegree < currentAngle) || (currentAngle <= 1)) {
				//Rotate Right
				OnAIMovementPressed(102, 304, 77);
				
			}
			//If ship is facing too far right
			else if ((360 - targetDegree > currentAngle)) {
				//Rotate Left
				OnAIMovementPressed(100, 304, 77);
				
			}
		}
		//South West Target
		else {
			//If ship is facing target
			if (((180 + targetDegree - deadzone) <= currentAngle) && (180 + targetDegree + deadzone) >= currentAngle) {
				//Make AI Decision
				AIDecision(c);
			}
			//IF AI is facing too far left
			else if (180 + targetDegree < currentAngle) {
				//Rotate Right
				OnAIMovementPressed(102, 304, 77);
				
			}
			//If ship is facing too far right
			else if (180 + targetDegree > currentAngle){
				//Rotate Left
				OnAIMovementPressed(100, 304, 77);
				
			}
		}
	}
	
}

//Decides if the AI Should shoot and/or move forward
void Asteroids::AIDecision(float c){
	//If the target is less than 90 away attempt to shoot
	if (c <= 90) {
		AIShoot();
	}
	//If target is further than 80 move forward
	if(c >= 80){
		OnAIMovementPressed(101, 257, 269);
	}
}

//Controls AI shooting
void Asteroids::AIShoot() {
	//If the AI's reload timer is 0
	if (reloadTimer == 0) { 
		//Shoot from spaceship
		mSpaceship.get()->Shoot(); 
		//Set reload timer to 10
		reloadTimer = 10;
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{	
		int bonusLife = rand() % 21;
		//If random generated bonusLife = 20 and the game is not transisioning from demo mode to player mode spawn a bonus life object

		if (bonusLife == 20 && ((demoMode == true && mLevel == -1) || (demoMode == false && mLevel != -1))) {
			//Set the bonus life's position to the position of the asteroid that was destroyed
			float xPosition = object->GetPosition().x;
			float yPosition = object->GetPosition().y;
			//Create instance of bonus life
			CreateBonusLife(xPosition, yPosition);
		}
		//Create and explosion on the position of the asteroid
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);

		//Remove asteroid from asteroid count
		mAsteroidCount--;
		//If there are no asteroids remaing begin next level
		if (mAsteroidCount <= 0) {
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
	//If object removed is of type BonusLife and the game is not in demo mode add a life to the player
	if (object->GetType() == GameObjectType("BonusLife")) {
		if (demoMode == false) {
			mPlayer.AddLife();
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	//Resets spaceship position and adds new spaceship to the gameworld
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}
	//Transitions the game from demo mode to playable mode
	if (value == CLEAR_DEMO){
		//Retrieves all objects present in the game world
		mGameObjects = mGameWorld->GetGameObjects();
		//Sets demo mode to false
		demoMode = false;
		//Removes the AIController listener
		mGameWorld->RemoveListener(&mAIController);
		//For all objects in the gameWorld
		for (GameObjectList::iterator it = mGameObjects.begin(); it != mGameObjects.end(); ++it) {
		
			shared_ptr<GameObject> thisPtr = (*it)->GetThisPtr();
			//If the object type is not spaceship simulate collision on the object
			if (thisPtr.get()->GetType().GetTypeName() != "Spaceship") {
				thisPtr.get()->OnCollision(mGameObjects);
			}
		}
		//Creates a new player
		SetTimer(500, CREATE_NEW_PLAYER);
		
	}
	//Begins next game level
	if (value == START_NEXT_LEVEL)
	{
		//sets default number of asteroids to 10
		int num_asteroids = 10 ;
		//If the game is not in demo mode increment the level count
		if (demoMode == false){
			mLevel++;
		}
		//If the game is starting playable mode
		if (mLevel == 0) {

			// Add a score keeper to the game world
			mGameWorld->AddListener(&mScoreKeeper);

			//Hides start screen labels
			mTitleLabel->SetVisible(false);
			mStartLabel->SetVisible(false);
			//Shows player data labels
			mScoreLabel->SetVisible(true);
			mLivesLabel->SetVisible(true);
		}
		
		if (mLevel >= 0) {
			//Changes the number of asteroids to be based on the level
			num_asteroids = 10 + (2 * mLevel);
		}
		//Creates asteroids 
		CreateAsteroids(num_asteroids);
	}

	//Game Over
	if (value == SHOW_GAME_OVER)
	{
		//Displays game over label
		mGameOverLabel->SetVisible(true);
	}

}

//If player killed
void Asteroids::OnPlayerKilled(int lives_left)
{
	//Character explosion
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	//If the game is in demo mode add a life to the player negating life lost so that AI can play continuously
	if (demoMode == true) {
		mPlayer.AddLife();
	}
	//Update life counter
	else {
		UpdateLifeCount(lives_left);
	}

	//If the player has lives left respawn player
	if (lives_left > 0) { SetTimer(1000, CREATE_NEW_PLAYER); }
	
	//If player is out of lives display game over
	else if (lives_left <= 0) {
		SetTimer(500, SHOW_GAME_OVER);
	}

}

//Updates player life count display
void Asteroids::UpdateLifeCount(int lives_left) {
	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

}

//Creates asteroids and adds animations and shapes to the asteroids
void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

//Creates BonusLife and adds animation and shape to the life
void Asteroids::CreateBonusLife(float xPosition, float yPosition) {
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("bonusLife");
	shared_ptr<Sprite> bonusLife_sprite
		= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	bonusLife_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> bonusLife = make_shared<BonusLife>(xPosition, yPosition);
	

	bonusLife->SetBoundingShape(make_shared<BoundingSphere>(bonusLife->GetThisPtr(), 3.0f));
	bonusLife->SetSprite(bonusLife_sprite);
	bonusLife->SetScale(0.03f);
	mGameWorld->AddObject(bonusLife);
}

void Asteroids::ResetSpaceship()
{
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);

	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);

	
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;
}


shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

void Asteroids::CreateGUI()
{
	/*Score GUI*/
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	//Set visiblity to false
	mScoreLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	/*Player GUI*/
	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	//Set visibility to false
	mLivesLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));



	// Create a new GUILabel and wrap it up in a shared_ptr
	mTitleLabel = shared_ptr<GUILabel>(new GUILabel("Asteroids"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Add the GUILabel to the GUIContainer
	shared_ptr<GUIComponent> title_component
		= static_pointer_cast<GUIComponent>(mTitleLabel);
	mGameDisplay->GetContainer()->AddComponent(title_component, GLVector2f(0.5f, 0.5f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mStartLabel = shared_ptr<GUILabel>(new GUILabel("Press ENTER to start"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Add the GUILabel to the GUIContainer
	shared_ptr<GUIComponent> start_component
		= static_pointer_cast<GUIComponent>(mStartLabel);
	mGameDisplay->GetContainer()->AddComponent(start_component, GLVector2f(0.5f, 0.45f));

}

