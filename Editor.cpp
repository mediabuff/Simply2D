#include <stdio.h>
#include "DropBox.h"
#include "Editor.h"
#include "StaticPlatform.h"
#include "MovingPlatform.h"
#include "WallJump.h"
#include "Enemy.h"
#include "Teleport.h"
#include "Trampoline.h"
#include "Spike.h"
#include "Turret.h"
#include "CameraManager.h"
#include "WindowHandler.h"
#include "ActivateButton.h"
#include "Gate.h"
#include "GunPowerup.h"
#include "Sound.h"

// Window beh�ver ingen mus l�ngre
// Editor ska ha den 

extern CameraManager* gCameraManager;
extern Sound* gSound;

Editor::Editor() : SNAP_SENSE(30), SNAP_DIST(20)
{
	mWindowHandler = new WindowHandler(1300, 450, 200, 900);
	mLevel = new Level(NULL);
	
	gameArea.top = 0;
	gameArea.bottom = GAME_HEIGHT;
	gameArea.left = 0;
	gameArea.right = GAME_WIDTH;

	mActiveObject = NULL;

	snapCount = SNAP_SENSE;
	snapDir = ALL;
	snappedObject = NULL;
	movingObject = false;
	movingEndPos = false;
	movingSpawnPos = false;
	movingWarp = false;
	showPaths = false;
	mObjectSnapped = false;
	tryLevel = false;
	currentAction = IDLE;
	mCtrlDown = false;
	mSnapping = true;

	createObjectTextPos = 290;
	mPrevActiveObjectType = NO_OBJECT;
}
Editor::~Editor()
{
	//mLevel->saveLevel("level_1.txt")
	mLevel->saveLevel((char*)mLevel->getLevelName().c_str());
	delete mLevel;
	delete mWindowHandler;
}


void Editor::addPropertyPair(Property prop)
{
	PropertyPair tmpPair;
	int y = 178 + 30 * mPropertyPairs.size();

	tmpPair.name = new TextBox(mWindowHandler, OBJECT_INFO, prop.name, 40, y, 60, 20);
	tmpPair.value = new InputBox(mWindowHandler, OBJECT_INFO, TYPE_FLOAT, 110, y, 60, 20, 5);
	tmpPair.value->setValue(prop.value);
	tmpPair.value->connect(&Editor::messageHandler, this);

	mPropertyPairs.push_back(tmpPair);

	mPropertyCount++;
}

void Editor::buildGUI(void)
{
	int OFFSET = 75;

	// should only allow input of 11 characters
	tLevel = new TextBox(mWindowHandler, TEXT_LEVEL, "Level:", 40, 22, 60, 20);
	iLevel = new InputBox(mWindowHandler, INPUT_LEVEL, TYPE_TEXT, 125, 22, 90, 20, 15);
	iLevel->connect(&Editor::messageHandler, this);
	iLevel->setValue("");

	tNextLevel = new TextBox(mWindowHandler, TEXT_NEXT, "Next:", 40, 50, 60, 20);
	iNextLevel = new InputBox(mWindowHandler, INPUT_NEXT, TYPE_TEXT, 125, 50, 90, 20, 15);
	iNextLevel->connect(&Editor::messageHandler, this);
	iNextLevel->setValue("");

	tSpawnX = new TextBox(mWindowHandler, TEXT_SPAWNX, "X:", 40, 95, 60, 20);
	tSpawnY = new TextBox(mWindowHandler, TEXT_SPAWNY, "Y:", 40, 125, 60, 20);

	iSpawnX = new InputBox(mWindowHandler, INPUT_SPAWNX, TYPE_FLOAT, 110, 95, 60, 20, 4);
	iSpawnX->connect(&Editor::messageHandler, this);

	iSpawnY = new InputBox(mWindowHandler, INPUT_SPAWNY, TYPE_FLOAT, 110, 125, 60, 20, 4);
	iSpawnY->connect(&Editor::messageHandler, this);

	// sets the values to the spawnPos!
	POS spawnPos = mLevel->getSpawn();
	sprintf(buffer, "%i", (int)mLevel->getSpawn().x);
	iSpawnX->setValue("");
	sprintf(buffer, "%i", (int)mLevel->getSpawn().y);
	iSpawnY->setValue("");

	listBox = new ListBox(mWindowHandler, LISTBOX_OBJECTTYPE, 76, 480 + OFFSET, 130, 235);	// shouldn't take height, should expand on addItem
	listBox->connect(&Editor::messageHandler, this);

	createButton = new Button(mWindowHandler, BUTTON_CREATE, "Create", 40, 662 + OFFSET, 60, 20, D3DCOLOR_ARGB(255, 90, 140, 140));
	createButton->connect(&Editor::messageHandler, this);
	createButton->setPressSound("misc\\sound\\menu_click.wav");

	deleteButton = new Button(mWindowHandler, BUTTON_DELETE, "Delete", 110, 662 + OFFSET, 60, 20, D3DCOLOR_ARGB(255, 90, 140, 140));
	deleteButton->connect(&Editor::messageHandler, this);

	saveButton = new Button(mWindowHandler, BUTTON_SAVE, "Save", 110, 692 + OFFSET, 60, 20, D3DCOLOR_ARGB(255, 90, 140, 140));
	saveButton->connect(&Editor::messageHandler, this);
	saveButton->setPressSound("misc\\sound\\menu_click.wav");

	bTryLevel = new Button(mWindowHandler, BUTTON_TRYLEVEL, "Test", 40, 692 + OFFSET, 60, 20, D3DCOLOR_ARGB(255, 90, 140, 140));
	bTryLevel->connect(&Editor::messageHandler, this);
	bTryLevel->setPressSound("misc\\sound\\menu_click.wav");

	textureDropBox = new DropBox(mWindowHandler, DROPBOX_TEXTURE, 76, 632 + OFFSET, 130, 20, 20);
	textureDropBox->connect(&Editor::messageHandler, this);

	pathCheckBox = new CheckBox(mWindowHandler, CHECKBOX_SHOWPATH, "Show paths: ", 110, 717 + OFFSET, 16, 16);
	pathCheckBox->connect(&Editor::messageHandler, this);

	mSnappingCheckBox = new CheckBox(mWindowHandler, CHECKBOX_SNAPPING, "Snapping ", 110, 737 + OFFSET, 16, 16);
	mSnappingCheckBox->setChecked(true);
	mSnappingCheckBox->connect(&Editor::messageHandler, this);

	listBox->addItem("Static Platform", 22, D3DCOLOR_ARGB( 255, 230, 230, 230 ));
	listBox->addItem("Moving Platform", 22, D3DCOLOR_ARGB( 255, 200, 200, 200 ));
	listBox->addItem("Enemy", 22, D3DCOLOR_ARGB( 255, 230, 230, 230));
	listBox->addItem("Teleport", 22, D3DCOLOR_ARGB( 255, 200, 200, 200));
	listBox->addItem("Trampoline", 22, D3DCOLOR_ARGB( 255, 230, 230, 230));
	listBox->addItem("Walljump", 22, D3DCOLOR_ARGB( 255, 200, 200, 200));
	listBox->addItem("Spike", 22, D3DCOLOR_ARGB( 255, 230, 230, 230));
	listBox->addItem("Turret", 22, D3DCOLOR_ARGB( 255, 200, 200, 200));
	listBox->addItem("Button", 22, D3DCOLOR_ARGB( 255, 230, 230, 230));
	listBox->addItem("Gate", 22, D3DCOLOR_ARGB( 255, 200, 200, 200));
	listBox->addItem("Gun Powerup", 22, D3DCOLOR_ARGB( 255, 230, 230, 230));

	textureDropBox->addItem("Dirt grass", D3DCOLOR_ARGB( 255, 200, 200, 200 ));
	textureDropBox->addItem("Dark grass", D3DCOLOR_ARGB( 255, 230, 230, 230 ));
}

int Editor::updateAll(float dt)
{
	if(tryLevel)
		return -1;

	mWindowHandler->update(dt);

	// the delta of the camera from it's orginal position ( center of the screen )
	mOffset = gCameraManager->gameCamera()->getOffset(); 
	POINT tmpMousePos = gDInput->getCursorPos();

	// add the camera offset to the mouse x coordinate
	tmpMousePos.x += mOffset;
	 
	
	MovingPlatform *tmpPlatform;

	// mousewheel scaling
	if(gDInput->mouseDZ() > 0)
	{
		if(mActiveObject != NULL)	{
			if(mActiveObject->getResizeable())	{
				mActiveObject->scale(ALL, 8, 8);

				messageHandler(OBJECT_UPDATED);
			}
		}
	}
	else if(gDInput->mouseDZ() < 0)	
	{
		if(mActiveObject != NULL)	{
			if(mActiveObject->getResizeable())	{
				if(mActiveObject->getHeight() > 50 && mActiveObject->getWidth() > 50)	{
					mActiveObject->scale(ALL, -8, -8);

					messageHandler(OBJECT_UPDATED);
				}
			}
		}
	}
		
	// select platform
	if(gDInput->mouseButtonPressed(LEFTBUTTON))	{
		// inside game area? (- mOffset because this test has to be in screen coordinates)
		if(tmpMousePos.x - mOffset> gameArea.left && tmpMousePos.x - mOffset < gameArea.right && tmpMousePos.y > gameArea.top)
		{	
			Object *tmpObject;
			tmpObject = NULL;
			bool sameObjectSelected = false;

			if(mActiveObject != NULL)
			{
				// if not selected the activeObject
				if(!(tmpMousePos.x > mActiveObject->getRect().left && tmpMousePos.x < mActiveObject->getRect().right
					&& tmpMousePos.y > mActiveObject->getRect().top && tmpMousePos.y < mActiveObject->getRect().bottom))
				{
					// if not selected the end rect or dest tp of the active object
					if(mActiveObject->getAreaAt(tmpMousePos.x, tmpMousePos.y) == OUTSIDE)
						tmpObject = mLevel->getObjectAt(tmpMousePos);
					else
						sameObjectSelected = true;
				}
				else	// if the active object is pressed
					sameObjectSelected = true;
			}
			else if(mActiveObject == NULL)
				tmpObject = mLevel->getObjectAt(tmpMousePos);

			// new object selected
			if(tmpObject != NULL && !sameObjectSelected)
			{				
				mActiveObject = tmpObject;
				tmpObject = NULL;
				messageHandler(OBJECT_SELECTED);
				mObjectSnapped = false;
			}
			// clicked in space
			else if(tmpObject == NULL && !sameObjectSelected)	{

				if(mActiveObject != NULL)	{
					if(mActiveObject->getAreaAt(tmpMousePos.x, tmpMousePos.y) != END_RECT)	{
						mActiveObject = NULL;
						mObjectSnapped = false;
					}
				}						
			}

			if(mActiveObject != NULL)	
			{
				movingSpawnPos = false;
				snapDir = ALL;
				mObjectSnapped = false;
				snapCount = SNAP_SENSE;
				messageHandler(OBJECT_SELECTED);	
			}		
			else
			{	
				// moving the spawn pos?
				RECT spawnRect = mLevel->getSpawnRect();
				if(tmpMousePos.x > spawnRect.left && tmpMousePos.x < spawnRect.right && tmpMousePos.y > spawnRect.top && tmpMousePos.y < spawnRect.bottom)	{
					movingSpawnPos = true;
				}
				else
					movingSpawnPos = false;

				RECT warpRect = mLevel->getWarp()->getRect();
				if(tmpMousePos.x > warpRect.left && tmpMousePos.x < warpRect.right && tmpMousePos.y > warpRect.top && tmpMousePos.y < warpRect.bottom)
					mActiveObject = mLevel->getWarp();
				
				// reset inputboxes
				resetInputBoxes();
			}
		}
		else
		{
			movingSpawnPos = false;
		}
	}
	// move, camera, resize, endPos of mActiveObject
	if(gDInput->mouseButtonDown(LEFTBUTTON))		
	{
		// move the spawn pos
		if(movingSpawnPos)	{
			moveSpawnPos();
			messageHandler(MOVE_SPAWNPOS);			
		}

		if(movingWarp)	{
			//mLevel->getWarp().x += gDInput->mouseDX();
			//mLevel->getWarp().y += gDInput->mouseDY();
		}

		if(mActiveObject != NULL)
		{
			RECT activeObjectRect = mActiveObject->getRect();
			ObjectArea areaType = mActiveObject->getAreaAt(tmpMousePos.x, tmpMousePos.y);			

			if(areaType != OUTSIDE || currentAction != IDLE)
			{
				// move dest tp
				// make a move(ObjectArea, dx, dy) function? will save some dynamic_casts!
				if((areaType == TP_DESTAREA && currentAction == IDLE) || currentAction == MOVING_TPDEST)
				{
					Teleport *tmpTp = dynamic_cast<Teleport*>(mActiveObject);
					tmpTp->moveDest(gDInput->mouseDX(), gDInput->mouseDY());
					currentAction = MOVING_TPDEST;
					messageHandler(OBJECT_UPDATED);
				}

				// resize it
				if(mActiveObject->getResizeable())
				{
					if((currentAction == SCALE_LEFT || currentAction == SCALE_RIGHT || currentAction == SCALE_UP || currentAction == SCALE_DOWN || currentAction == IDLE)
						|| (areaType == DRAG_LEFT || areaType == DRAG_RIGHT || areaType == DRAG_UP || areaType == DRAG_DOWN))
					{
							if(currentAction == SCALE_LEFT)
								resizePlatform(DRAGLEFT);
							else if(currentAction == IDLE && areaType == DRAG_LEFT)	{
									resizePlatform(DRAGLEFT);
									currentAction = SCALE_LEFT;
							}
							else if(currentAction == SCALE_RIGHT)
								resizePlatform(DRAGRIGHT);
							else if(currentAction == IDLE && areaType == DRAG_RIGHT)	{
									resizePlatform(DRAGRIGHT);
									currentAction = SCALE_RIGHT;
							}
							else if(currentAction == SCALE_UP)
								resizePlatform(DRAGUP);
							else if(currentAction == IDLE && areaType == DRAG_UP)	{
									resizePlatform(DRAGUP);
									currentAction = SCALE_UP;
							}
							else if(currentAction == SCALE_DOWN)
								resizePlatform(DRAGDOWN);
							else if(currentAction == IDLE && areaType == DRAG_DOWN)	{
									resizePlatform(DRAGDOWN);
									currentAction = SCALE_DOWN;
							}
					}
				}
				// move the object
				if(currentAction == MOVING_OBJECT)
				{
					moveObject();					
					currentAction = MOVING_OBJECT;
				}
				else if(areaType == BODY && currentAction != SCALE_LEFT && currentAction != SCALE_RIGHT && currentAction != SCALE_UP && currentAction != SCALE_DOWN)
				{
					moveObject();					
					currentAction = MOVING_OBJECT;	
				}	

				// move the endpos
				if(currentAction == MOVING_ENDPOS)
					moveEndPos();
				else if(currentAction == IDLE && areaType == END_RECT)	{
					currentAction = MOVING_ENDPOS;
					moveEndPos();
				}
			}
		}
	}

	// scroll knapp nere -> r�r kamera
	if(gDInput->mouseButtonDown(MIDDLEBUTTON))
	{
		// shouldn't be able to move outside to the left
		if(gCameraManager->gameCamera()->getX() > 600 || (gCameraManager->gameCamera()->getX() == 600 && gDInput->mouseDX() < 0))	{
			gCameraManager->gameCamera()->addMovement(-gDInput->mouseDX(), 0);
		}
		
	}

	// set the currentAction to IDLE
	if(gDInput->mouseButtonReleased(LEFTBUTTON) && currentAction != IDLE)
	{
			currentAction = IDLE;
	}

	

	return 1;
}
// k�rs n�r man tar tag i markerad plattform
void Editor::moveObject(void)
{		
	RECT activeObjectRect = mActiveObject->getRect();

	// updatera koordinater
	//float dx = gMouse->getDX();
	//float dy = gMouse->getDY();
	float dx = gDInput->mouseDX();
	float dy = gDInput->mouseDY();
				
			
	// om det inte �r n�gon snapping	-> kolla snapping
	// �r det snapping					-> snap + f�rhindra movement
	// �r det inte snapping				-> flytta normalt
	if(!mObjectSnapped)
	{
		mObjectSnapped = objectSnapping(mActiveObject, dx, dy);
		if(!mObjectSnapped) // can change in objectSnapping()
			mActiveObject->editorMove(dx, dy);
	}
	else
	{
		if(snapDir == LEFT || snapDir == RIGHT)
		{
			// dx kan vara b�da + att om den inte �r n�ra snapped ska den r�ra sig fritt
			if(snapCount > SNAP_SENSE || snapCount < -SNAP_SENSE )
			{				
				mActiveObject->editorMove(dx, dy);

				snapDir = ALL;
				mObjectSnapped = false;
				snapCount = 0;
			}
			else	{	// snapped, don't move the object or mouse						
				snapCount += dx;
				gDInput->setCursorX(gDInput->getCursorX() - dx);	// dont move the mouse
				mActiveObject->editorMove(0, dy);	// allow movement in the oppisite direction (up/down)
			}
		}
		else if(snapDir == UP || snapDir == DOWN)
		{
			if(snapCount >= SNAP_SENSE || snapCount <= -SNAP_SENSE)
			{		
				mActiveObject->editorMove(dx, dy);

				snapDir = ALL;
				mObjectSnapped = false;
				snapCount = 0;
			}
			else	{
				snapCount += dy;
				gDInput->setCursorY(gDInput->getCursorY() - dy);
				mActiveObject->editorMove(dx, 0);
				
			}					
		}			
	}

	messageHandler(OBJECT_UPDATED);
}

bool Editor::objectSnapping(Object *object, float dx, float dy)
{
	if(mSnapping)
	{
		if(mLevel->objectIntersection(mActiveObject) == NULL)	{
		
			snapDir = mLevel->snapObject(object, SNAP_DIST);
		
			if(snapDir != ALL)	
			{	
				snapCount = 0;
				return true;
			}
		}
	}

	return false;
}

// just renders the GUI to the right
int Editor::renderGui()
{
	// the green side
	gGraphics->BlitRect(1300, 450, 200, 900, D3DCOLOR_ARGB( 155, 155, 200, 000));

	mWindowHandler->draw();

	gGraphics->drawText("Spawn:", GAME_WIDTH +10, 65);
	gGraphics->drawText("Active object:", GAME_WIDTH +10, 140);
	gGraphics->drawText("Create object:", GAME_WIDTH +10, 407);

	return 1;
}

void Editor::resetInputBoxes(void)
{
	for(int i = 0; i < mPropertyPairs.size(); i++)
	{
		mPropertyPairs[i].value->setValue("");
	}
}

void Editor::resizePlatform(DragRect drag)
{
	// updatera movements
	float dx = gDInput->mouseDX();
	float dy = gDInput->mouseDY();

	RECT activeObjectRect = mActiveObject->getRect();

	if(drag == DRAGLEFT)
	{
		if((mActiveObject->getWidth() >= 50 && dx > 0) || dx < 0)	{
			activeObjectRect.left += dx;
			mActiveObject->scale(LEFT, dx, 0);		
		}
		else
			gDInput->setCursorX(gDInput->getCursorX() - dx);
	}	
	else if(drag == DRAGRIGHT)
	{
		if((mActiveObject->getWidth() >= 50 && dx < 0) || dx > 0)	{
			activeObjectRect.right += dx;
			mActiveObject->scale(RIGHT, dx, 0);		// drar man musen �t h�ger �kar bredden
		}
		else
			gDInput->setCursorX(gDInput->getCursorX() - dx);
	}
	else if(drag == DRAGUP)
	{
		if((mActiveObject->getHeight() >= 50 && dy > 0) || dy < 0)	{
			activeObjectRect.top += dy;
			mActiveObject->scale(UP, 0, -dy);
		}
		else
			gDInput->setCursorY(gDInput->getCursorY() - dy);
	}
	else if(drag == DRAGDOWN)
	{	
		if((mActiveObject->getHeight() >= 50 && dy < 0) || dy > 0)	{
			activeObjectRect.bottom += dy;
			mActiveObject->scale(DOWN, 0, dy);
		}
		else
			gDInput->setCursorY(gDInput->getCursorY() - dy);
	}

	messageHandler(OBJECT_UPDATED);
}

bool Editor::stillSnapped(void)
{		
	if(snappedObject != NULL)
	{
		RECT activeObjectRect = mActiveObject->getRect();
		RECT snappedObjectRect = snappedObject->getRect();
		if(activeObjectRect.top <= snappedObjectRect.bottom && activeObjectRect.bottom >= snappedObjectRect.top && activeObjectRect.right >= snappedObjectRect.left &&
			activeObjectRect.left <= snappedObjectRect.right && mActiveObject->getID() != snappedObject->getID())	// ville nog egentligen ta reda p� om id = last snap id
			return true;
	}

	return false;
}

bool Editor::messageHandler(WindowID id, WindowMessage msg)
{
	char temp[256];

	switch(id)
	{
	case OBJECT_INFO:
		{
			if(mActiveObject != NULL)
			{
				// load properties into the active object
				// a vector of properties have to be sent to the object
				std::vector<Property> propertyList;

				// build property list from the information of the widgets
				Property tmpProperty;
				for(int i = 0; i < mPropertyPairs.size(); i++)	{
					tmpProperty.name = mPropertyPairs[i].name->getValue();
					tmpProperty.value = mPropertyPairs[i].value->getString();
					propertyList.push_back(tmpProperty);
				}
				mActiveObject->loadProperties(propertyList);

			}
			break;
		}
	case INPUT_SPAWNX:
		{
			POS spawn = mLevel->getSpawn();
			spawn.x = msg.getInt();
			mLevel->setSpawn(spawn);
			break;
		}
	case INPUT_SPAWNY:
		{
			POS spawn = mLevel->getSpawn();
			spawn.y =msg.getInt();
			mLevel->setSpawn(spawn);
			break;
		}
	case DROPBOX_TEXTURE:
		{
			if(mActiveObject != NULL)
			{
				if(mActiveObject->getType() == STATIC_PLATFORMA || mActiveObject->getType() == MOVING_PLATFORM)	{
					if(textureDropBox->getValue() == "Dark grass")
						mActiveObject->setTextureSource("misc\\textures\\grass_platform.bmp");
					if(textureDropBox->getValue() ==  "Dirt grass")
						mActiveObject->setTextureSource("misc\\textures\\dirt_grass.bmp");
				}
			}
			break;
		}
	case BUTTON_CREATE:
		{
			createObject(METHOD_BUTTON);
			break;
		}
	case KEY_CREATE:
		{
			createObject(METHOD_MOUSE);
			break;
		}
	case BUTTON_DELETE:
		{		
			if(mActiveObject != NULL && mActiveObject->getType() != LEVEL_WARP)	{
					mLevel->removeObject(mActiveObject->getID());	// ska l�gga till f�r dynamic ocks�!

				resetInputBoxes();
				mActiveObject = NULL;
				gSound->playEffect("misc\\sound\\delete.wav");
			}
			break;
		}
	case BUTTON_SAVE:
		{
			// levels save function does the work
			strcpy(buffer, iLevel->getString().c_str());

			// set the levels next level
			string tmp = "levels\\";
			tmp.append(iNextLevel->getString());
			tmp.append(".txt");
			mLevel->setNextLevel(tmp);

			// rename the filename and the level name if the name has changed
			string compareString = "levels\\";
			compareString.append(iLevel->getString());
			compareString.append(".txt");
			if(mOldLevelName != compareString)	{
				mLevel->setLevelName(compareString);

				string newName = "levels\\";
				newName.append(iLevel->getString());
				newName.append(".txt");

				if(rename(mOldLevelName.c_str(), newName.c_str()) != 0)
					MessageBox(0, "heer", 0, 0); 			
			}

			// add  .txt
			//tmp = "levels\\";
			//tmp.append(string(buffer));
			//tmp.append(".txt");
			mLevel->saveLevel((char*)compareString.c_str());
			break;
		}
	case BUTTON_TRYLEVEL:
		{
			string tmp = "levels\\";
			tmp.append(iLevel->getString().c_str());
			tmp.append(".txt");

			gDInput->restoreCursor();
			gCameraManager->gameCamera()->restore();
			mLevel->saveLevel((char*)tmp.c_str());
			tryLevel = true;
			break;
		}
	case OBJECT_SELECTED:
		{
			if(mActiveObject != NULL)
			{
				// delete old property widgets
				// remove old propertyPair list
				for(int i = 0; i < mPropertyPairs.size(); i++)	{
					mWindowHandler->removeWindow(mPropertyPairs[i].name);
					mWindowHandler->removeWindow(mPropertyPairs[i].value);

					mPropertyPairs[i].name = NULL;
					mPropertyPairs[i].value = NULL;
				}

				mPropertyPairs.clear();

				std::vector<Property> properties = mActiveObject->getProperties();

				for(int i = 0; i < properties.size(); i++)
				{
					addPropertyPair(properties[i]);			
				}

				// set the texture dropbox value
				if(mActiveObject->getType() == STATIC_PLATFORMA || mActiveObject->getType() == MOVING_PLATFORM)	{
					if(strcmp(mActiveObject->getTextureSource(), "misc\\textures\\dirt_grass.bmp") == 0)
						textureDropBox->setValue("Dirt grass");
					else if(strcmp(mActiveObject->getTextureSource(), "misc\\textures\\grass_platform.bmp") == 0)
						textureDropBox->setValue("Dark grass");
				}
			}
			
			break;
		}
	case OBJECT_DESELECTED:
		{
			// empty widgets on information about the object
			mActiveObject = NULL;
			resetInputBoxes();
			break;
		}
	case CHECKBOX_SHOWPATH:
		{
			showPaths = msg.getBool();//getChecked();
			break;
		}
	case CHECKBOX_SNAPPING:
		{
			mSnapping = msg.getBool();
			break;
		}
	case MOVE_SPAWNPOS:
		{
			// spawnPos 
			iSpawnX->setValue((int)mLevel->getSpawn().x);
			iSpawnY->setValue((int)mLevel->getSpawn().y);
			break;
		}
	case OBJECT_UPDATED:
		{
			// update the input boxes with the new information!
			updatePropertyWidgets();
			break;
		}
	case COPY_OBJECT:
		{
			if(mActiveObject == NULL)
				break;

	
			if(mActiveObject->getType()  == STATIC_PLATFORMA)
			{			
				StaticPlatform *tmpObject = new StaticPlatform(*dynamic_cast<StaticPlatform*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == MOVING_PLATFORM)
			{			
				MovingPlatform *tmpObject = new MovingPlatform(*dynamic_cast<MovingPlatform*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == NORMAL_ENEMY)
			{			
				Enemy *tmpObject = new Enemy(*dynamic_cast<Enemy*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == TELEPORT)
			{			
				Teleport *tmpObject = new Teleport(*dynamic_cast<Teleport*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == TRAMPOLINE)
			{			
				Trampoline *tmpObject = new Trampoline(*dynamic_cast<Trampoline*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == WALLJUMP)
			{			
				WallJump *tmpObject = new WallJump(*dynamic_cast<WallJump*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == SPIKE)
			{			
				Spike *tmpObject = new Spike(*dynamic_cast<Spike*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == TURRET)
			{			
				Turret *tmpObject = new Turret(*dynamic_cast<Turret*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == ACTIVATE_BUTTON)
			{			
				ActivateButton *tmpObject = new ActivateButton(*dynamic_cast<ActivateButton*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == GATE)
			{			
				Gate *tmpObject = new Gate(*dynamic_cast<Gate*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
			else if(mActiveObject->getType()  == GUNPOWERUP)
			{			
				GunPowerup *tmpObject = new GunPowerup(*dynamic_cast<GunPowerup*>(mActiveObject));//Object* tmpObject = new Object(*mActiveObject);;

				tmpObject->setXY(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - tmpObject->getWidth()/2, gDInput->getCursorY() - tmpObject->getHeight()/2);
				mLevel->addObject(tmpObject);
			}
		}
	}
	
	return true;
}

void Editor::moveEndPos(void)
{
	float dx = gDInput->mouseDX();
	float dy = gDInput->mouseDY();
	POS endPos;

	// when this function gets called we know that the active object is a moving one
	// therefor we type cast it in order to reach getEndPos() and setEndPos()

	MovingObject *tmpObject = dynamic_cast<MovingObject*>(mActiveObject);

	endPos = tmpObject->getEndPos();
	endPos.x += dx;
	tmpObject->setEndPos(endPos);

	messageHandler(OBJECT_UPDATED);
}

void Editor::moveSpawnPos(void)
{
	float dx = gDInput->mouseDX();
	float dy = gDInput->mouseDY();

	POS spawnPos = mLevel->getSpawn();

	spawnPos.x += dx;
	spawnPos.y += dy;

	mLevel->setSpawn(spawnPos);
}

int Editor::renderLevel(void)
{
	if(showPaths)
		mLevel->drawEditorLevel();		// hax ._-
	else if(!showPaths)
		mLevel->drawLevel();

	// draw the spawn pos
	POS spawnPos = mLevel->getSpawn();
	gGraphics->BlitRect(spawnPos.x, spawnPos.y, USER_WIDTH, USER_HEIGHT, D3DCOLOR_ARGB(220, 220, 40, 0));

	// draw the activeObject orange effect and end pos + dest tp etc..
	if(mActiveObject != NULL)	{
		mActiveObject->drawEditorFX();
		gGraphics->BlitRect(mActiveObject->getRect(), D3DCOLOR_ARGB(150, 255, 166, 0));	
	}

	return 1;
}

void Editor::updatePropertyWidgets(void)
{
	std::vector<Property> activeObjectProperties = mActiveObject->getProperties();
	for(int i = 0; i < mPropertyPairs.size(); i++)
	{
		mPropertyPairs[i].value->setValue(activeObjectProperties[i].value);
	}
}

void Editor::loadLevel(char *source) 
{
	mLevel->loadLevel(source);

	// remove level\ and .txt
	string tmp = string(source);
	mOldLevelName = tmp;
	tmp.erase(tmp.size()-4, tmp.size());
	tmp.erase(0, 7);
	iLevel->setValue(tmp);

	mLevel->getSpawn().x;

	// hax warning

	itoa(mLevel->getSpawn().x, buffer, 10);
	tmp = string(buffer);
	iSpawnX->setValue(tmp);

	itoa(mLevel->getSpawn().y, buffer, 10);
	tmp = string(buffer);
	iSpawnY->setValue(tmp);

	// load the nextmap and display it
	// remove level\ and .txt
	tmp = mLevel->getNextLevel();
	tmp.erase(0, 7);
	tmp.erase(tmp.size()-4, tmp.size());

	iNextLevel->setValue(tmp);
}

string Editor::getTestLevel(void)
{
	string tmp = mLevel->getLevelName();
	return tmp;
}

void Editor::keyPressed(WPARAM wParam)
{
	// hack - levels can't have X in their names
	if(wParam == 'X')
		messageHandler(BUTTON_DELETE);
	else	{
		if(wParam == VK_DELETE)
			messageHandler(BUTTON_DELETE);
		else if(wParam == 'C' && !mCtrlDown)
			messageHandler(KEY_CREATE);
		else if(wParam == 'C' && mCtrlDown)
			messageHandler(COPY_OBJECT);
		else if(wParam == 'T' && mCtrlDown)
			messageHandler(BUTTON_TRYLEVEL);
		else if(wParam == 'S' && mCtrlDown)
			messageHandler(BUTTON_SAVE);
		else if(wParam == VK_CONTROL)
			ctrlDown(true);

		mWindowHandler->keyPressed(wParam);	
	}
	
}

void Editor::createObject(CreateMethod method)
{
	// this basicly creates the object which is marked in the object listbox
	// the most of the cluttering in here is to find out the right coordinates
	// there is two ways; fixed position or mouse position
	string value = listBox->getValue(); 
	if(value != "none")
	{		
		if(value == "Static Platform")
		{			
			StaticPlatform *platform;
			if(method == METHOD_BUTTON)
				platform = new StaticPlatform(gCameraManager->gameCamera()->getOffset() + 600, 400, 100, 100, "misc\\textures\\dirt_grass.bmp");
			else
				platform = new StaticPlatform(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 50, gDInput->getCursorY() - 50, 100, 100, "misc\\textures\\dirt_grass.bmp");
			mLevel->addObject(platform);
		}
		else if(value == "Moving Platform")
		{		
			MovingPlatform *platform;
			if(method == METHOD_BUTTON)	{
				POS start(gCameraManager->gameCamera()->getOffset() + 200, 300);
				POS end(gCameraManager->gameCamera()->getOffset() + 600, 300);
				platform = new MovingPlatform(gCameraManager->gameCamera()->getOffset() + 200, 300, 100, 100, "misc\\textures\\grass_platform.bmp", start, end);
				
			}
			else	{
				POS start(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 50, gDInput->getCursorY() - 50);
				POS end(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() + 200 - 50, gDInput->getCursorY() - 50);
				platform = new MovingPlatform(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 50, gDInput->getCursorY() - 50, 100, 100, "misc\\textures\\grass_platform.bmp", start, end);
			}
					
			mLevel->addObject(platform);
		}
		else if(value == "Enemy")
		{
			Enemy *enemy;
			
			if(method == METHOD_BUTTON)	{
				POS start(gCameraManager->gameCamera()->getOffset() + 200, 300);
				POS end(gCameraManager->gameCamera()->getOffset() + 600, 300);
				enemy = new Enemy(gCameraManager->gameCamera()->getOffset() + 200, 500, 36, 36, "misc\\textures\\bad_mario.bmp", start, end);
				
			}
			else	{
				POS start(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 18, gDInput->getCursorY() - 18);
				POS end(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() + 200 - 18, gDInput->getCursorY()-18);
				enemy = new Enemy(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 18, gDInput->getCursorY() - 18, 36, 36, "misc\\textures\\bad_mario.bmp", start, end);
			}
			mLevel->addObject(enemy);
		}
		else if(value == "Teleport")
		{
			Teleport *teleport;
			if(method == METHOD_BUTTON)
				teleport = new Teleport(gCameraManager->gameCamera()->getOffset() + 200, 500, 600, 300, 50, 50, (char*)TP_ENTER.c_str(), (char*)TP_DEST.c_str());
			else 
				teleport = new Teleport(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 25, gDInput->getCursorY() - 25, 600, 300, 50, 50, (char*)TP_ENTER.c_str(), (char*)TP_DEST.c_str());
			mLevel->addObject(teleport);
		}
		else if(value == "Trampoline")
		{
			Trampoline *trampoline;
			if(method == METHOD_BUTTON)
				trampoline = new Trampoline(gCameraManager->gameCamera()->getOffset() + 300, 300, 40, 32, 120, 20, "misc\\textures\\trampoline.bmp");
			else
				trampoline = new Trampoline(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 40, gDInput->getCursorY() - 18, 40, 32, 120, 20, "misc\\textures\\trampoline.bmp");
			mLevel->addObject(trampoline);
		}
		else if(value == "Walljump")
		{
			WallJump *walljump;
			if(method == METHOD_BUTTON)
				walljump = new WallJump(gCameraManager->gameCamera()->getOffset() + 300, 300, 60, 60, "misc\\textures\\walljump.bmp");
			else
				walljump = new WallJump(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 30, gDInput->getCursorY() - 30, 60, 60, "misc\\textures\\walljump.bmp");
			mLevel->addObject(walljump);
		}
		else if(value == "Spike")
		{
			Spike *spike;
			if(method == METHOD_BUTTON)
				spike = new Spike(gCameraManager->gameCamera()->getOffset() + 300, 300, 50, 50, "misc\\textures\\spike.bmp", 100);
			else
				spike = new Spike(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 25, gDInput->getCursorY() - 25, 50, 50, "misc\\textures\\spike.bmp", 100);
			mLevel->addObject(spike);
		}
		else if(value == "Turret")
		{
			Turret *turret;
			if(method == METHOD_BUTTON)
				turret = new Turret(gCameraManager->gameCamera()->getOffset() + 300, 300, 40, 40, (char*)TURRET_SOURCE.c_str(), 100, LEFT, 50, 100, 200, .5);
			else
				turret = new Turret(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 20, gDInput->getCursorY() - 20, 40, 40, (char*)TURRET_SOURCE.c_str(), 100, LEFT, 50, 100, 200, .5);
			mLevel->addObject(turret);
		}
		else if(value == "Button")
		{
			ActivateButton *button;
			if(method == METHOD_BUTTON)
				button = new ActivateButton(gCameraManager->gameCamera()->getOffset() + 300, 300, 30, 60, (char*)ACTIVATEBUTTON_UNPRESSED_SOURCE.c_str());
			else
				button = new ActivateButton(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 15, gDInput->getCursorY() - 30, 30, 60, (char*)ACTIVATEBUTTON_UNPRESSED_SOURCE.c_str());
			button->setLevel(mLevel);
			mLevel->addObject(button);
		}
		else if(value == "Gate")
		{
			Gate *gate;
			if(method == METHOD_BUTTON)
				gate = new Gate(gCameraManager->gameCamera()->getOffset() + 300, 300, 40, 60, (char*)GATE_SOURCE.c_str(), 3);
			else
				gate = new Gate(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 20, gDInput->getCursorY() - 30, 40, 60, (char*)GATE_SOURCE.c_str(), 3);
			mLevel->addObject(gate);
		}
		else if(value == "Gun Powerup")
		{
			GunPowerup *powerup;
			if(method == METHOD_BUTTON)
				powerup = new GunPowerup(gCameraManager->gameCamera()->getOffset() + 300, 300, 40, 40, (char*)MAP_GUN_SOURCE.c_str(), 5);
			else
				powerup = new GunPowerup(gCameraManager->gameCamera()->getOffset() + gDInput->getCursorX() - 20, gDInput->getCursorY() - 20, 40, 40, (char*)MAP_GUN_SOURCE.c_str(), 5);
			mLevel->addObject(powerup);
		}
	}
		// aktiv plattform = den nya?
}

void Editor::ctrlDown(bool down)
{
	mCtrlDown = down;
}