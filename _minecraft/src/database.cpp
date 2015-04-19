#include "database.h"
#include "gameobject.h"


Database::Database( void )
{
	nextFreeID = 1;
}
	

Database::~Database( void )
{

}


void Database::Store( GameObject & object )
{
	if( Find( object.GetID() ) == 0 ) {
		database.push_back( &object );
	}
	else {
		assert( !"Database::Store - Object ID already represented in database." );
	}
}


void Database::Remove( objectID id )
{
	dbContainer::iterator i;
	for( i=database.begin(); i!=database.end(); ++i )
	{
		if( (*i)->GetID() == id ) {
			database.erase(i);	
			return;
		}
	}

	return;
}


GameObject* Database::Find( objectID id )
{
	dbContainer::iterator i;
	for( i=database.begin(); i!=database.end(); ++i )
	{
		if( (*i)->GetID() == id ) {
			return( *i );
		}
	}

	return(0);

}


objectID Database::GetNewObjectID( void )
{
	return( nextFreeID++ );

}
