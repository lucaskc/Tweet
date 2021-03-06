/*
 * @Authors:
 *
 * Frederico de Azevedo Marques - 8936926
 * Henrique Martins Loschiavo   - 8936972
 * Lucas Kassouf Crocomo        - 8937420
 * Roberto Pommella Alegro      - 8936756
 *
 */

#include "tweet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Useful defines
#define CMD_LENGTH 30

#define CMD_INSERT "insert"
#define CMD_EXIT "exit"
#define CMD_REMOVE "remove"
#define CMD_REQUEST "request"
#define CMD_REQUEST_ALL "all"
#define CMD_REQUEST_RRN "rrn"
#define CMD_REQUEST_USER "user"
#define CMD_CREDITS "credits"
#define CMD_LS "ls"
#define CMD_CD "cd"
#define CMD_PWD "pwd"
#define CMD_MKDIR "mkdir"
#define CMD_CHANGE_DATAFILE "cf"
#define CMD_CLEAR "clear"
#define CMD_HELP "help"

#define SPEC_PDF "http://wiki.icmc.usp.br/images/6/63/SCC0215012015projeto01TurmaBa.pdf"
#define DOC_PDF "INSERT NAME HERE"
#define PROFESSOR_NAME "Cristina Ciferre"


// OS dependent functions
#ifdef _WIN32
#define CLEAR "cls"
#define LS "dir"
#define SEP "\\"
#define CHANGEDIR(X) _chdir(X)
#define WORKINGDIRECTORY(X, Y) _getcwd(X, Y)
#define MKDIRECTORY(X, Y) _mkdir(X)
#include <direct.h>
#else
#define CLEAR "clear"
#define LS "ls"
#define SEP "/"
#define CHANGEDIR(X) chdir(X)
#define WORKINGDIRECTORY(X, Y) getcwd(X, Y)
#define MKDIRECTORY(X, Y) mkdir(X, Y)
#include <unistd.h>
#include <sys/stat.h>
#endif

enum {

	PROGNAME,	//0
	FILENAME	//1

};

/**
 * A function that recieves from the user the information to add to the file and calls
 * the function insertTweet, from the library implementation
 *
 * @param fileName - the name of the currently in use file
 *
 * @variable user - the userName to be inserted
 * @variable lang - the language to be inserted
 * @variable coords - the coordinates to be inserted
 * @variable text - the text to be inserted
 * @variable fav - the favorite count to be inserted
 * @variable retweet - the retweet count to be inserted
 * @variable view - the number of views to be inserted
 * @variable ret - stores the return value of the insert function for further use
 *
 */
void cmdInsert(char *fileName){
	char user[USER_SIZE];
	char lang[LANGUAGE_SIZE];
	char coords[COORDINATES_SIZE];
	char text[TEXT_SIZE];
	int fav, retweet, ret;
	long view;

	//request all information needed to insert a new tweet
	printf("Please type a user name:\n");
	scanf("\n"); //read a \n that is still in the buffer from the previous scanf
	fgets(user, sizeof user, stdin);
	user[strlen(user)-2] = 0; //fgets reads the \n to the string, so we need to remove it. Otherwise it would be inserted to the database
	printf("Type your geographic coordinates:\n");
	fgets(coords, sizeof coords, stdin);
	coords[strlen(coords)-2] = 0;
	printf("Specify the language of your text:\n");
	fgets(lang, sizeof lang, stdin);
	lang[strlen(lang)-2] = 0;
	printf("Type your text:\n");
	fgets(text, sizeof text, stdin);
	text[strlen(text)-2] = 0;
	printf("Type how many favorites your tweet has:\n");
	scanf("%d", &fav);
	printf("Type how many views your tweet has:\n");
	scanf("%ld", &view);
	printf("Type how many retweets your tweet has:\n");
	scanf("%d", &retweet);

	// call the function that will insert the tweet into the database
	ret = insertTweet(fileName, text, user, coords, fav, lang, retweet, view);

	// check the return value and print a error message if needed
	if(ret == SUCCESS){
		printf("Tweet inserted successfully!\n");
	}else{
		printf("Could not insert the tweet. Is the filesystem read only?\n");
	}
}

/**
 * A function that gets the RRN and calls the removeTweet function
 * to remove a tweet
 *
 * @param fileName - the name of the currently in use file
 *
 * @variable RRN - the relative register number (ID) to be removed from the database
 * @variable ret - stores the return value of the removal function for further use
 *
 */
void cmdRemove(char *fileName){
	int RRN, ret;

	printf("Type the ID (RRN) of the Tweet to be removed: ");
	scanf("%d", &RRN);

	// call the function to remove the tweet using the RRN
	ret = removeTweet(fileName, RRN);
	// check the return value and print a error message if needed
	if(ret == SUCCESS){
		printf("The Tweet %d was successfully removed\n", RRN);
	}else{
		printf("The Tweet %d could not be removed. Maybe it does not exist\n", RRN);
	}
}

/**
 * A function that requests tweet data and print to screen, depending on which tweets the user
 * requests (all, one or all from a single user)
 *
 * @param fileName - the name of the currently in use file
 *
 * @variable RRN - the relative register number (ID) to be requested from the database
 * @variable tweets - a array of tweets returned from the requestAll and findTweetByUser functions
 * @variable tweet - a single tweet returned from the requestTweet function
 * @variable cmd - the subcommand typed after the "request" command
 * @variable buf - a buffer, to consume the stdin buffer in case of a invalid command
 * @variable RRN - the RRN to use as parameter to the requestTweet function
 * @variable ammount - the ammount of tweets returned by the requestAll or findTweetByUser functions
 * @variable i - a dummy counter
 *
 */
void cmdRequest(char *fileName){
	TWEET **tweets = NULL;
	TWEET *tweet = NULL;
	char cmd[CMD_LENGTH], buf[10*CMD_LENGTH];
	int RRN, ammount = 0, i;

	// reads the next part of the request command
	scanf("%s", cmd); 
	// a "switch" structure 
	if(strcmp(cmd, CMD_REQUEST_ALL) == 0){
		tweets = requestAllTweets(fileName, &ammount); // get a array of all tweets and the ammount of tweets
	}else if(strcmp(cmd, CMD_REQUEST_USER) == 0){
		scanf("%s", cmd); // reads the user to find for
		tweets = findTweetByUser(fileName, cmd, &ammount); // get a array of all tweets from that user and the ammount of tweets
	}else if(strcmp(cmd, CMD_REQUEST_RRN) == 0){
		scanf("%d", &RRN); // reads the RRN to request from
		tweet = requestTweet(fileName, RRN); // get a tweet
	}else{
		fgets(buf, sizeof buf, stdin); // if the command is invalid, consume the stdin buffer
		buf[strlen(buf)-1] = 0; // as fgets adds the \n to the buffer, we remove it
		printf("Invalid command: %s%s, try typing '%s' for help\n", cmd, buf, CMD_HELP); // says that the command is not valid
	}

	if(tweets == NULL && tweet == NULL){ // if no tweets were found, print a error message
		printf("Could not find requested Tweet(s). Maybe it does not exist\n");
	}else{
		if(tweet == NULL){ // check if the ran command returned a array of tweets or a single one
			for(i = 0; i < ammount; i++){ // print the array of tweets
				printf(printTweet(tweets[i]));
				printf("---------------------------------------------------\n");
				free(tweets[i]); // free every tweet
			}
			free(tweets); // free the base structure
		}else{
			printf(printTweet(tweet)); // print a single tweet
			printf("---------------------------------------------------\n");
			free(tweet); // free that tweet
		}
	}
}

/**
 * A function that print this application credits
 */
void cmdCredits(){
	printf("---------------------------------------------------\n");
	printf("This program was made by:\n\
\n\
* Frederico de Azevedo Marques - N USP: 8936926\n\
* Henrique Martins Loschiavo   - N USP: 8936972\n\
* Lucas Kassouf Crocomo        - N USP: 8937420\n\
* Roberto Pommella Alegro      - N USP: 8936756\n\
\n\
Using the specification available at:\n\
%s\n\
and the knowledge obtained from the classes of professor %s\n\
---------------------------------------------------\n", SPEC_PDF, PROFESSOR_NAME);
}

/**
 * A function that provides a help chart for this application
 *
 * @param progname - the name of this application, as it was executed
 *
 */
void cmdHelp(char *progname){
	printf("---------------------------------------------------\n");
	printf("Usage: %s fileName.dat\n", progname);
	printf("\n"); 
	printf("%s\t\t\t- print this help\n", CMD_HELP);
	printf("%s\t\t\t- print the credits\n", CMD_CREDITS);
	printf("%s\t\t\t- insert a tweet into the database (You will be prompted asking for the information to store)\n", CMD_INSERT);
	printf("%s <RRN>\t\t- remove a tweet from the database using it's ID(RRN)\n", CMD_REMOVE);
	printf("%s %s\t\t- print all tweets in the database\n", CMD_REQUEST, CMD_REQUEST_ALL);
	printf("%s %s <UserName>\t- print all tweets of the user <UserName>\n", CMD_REQUEST, CMD_REQUEST_USER);
	printf("%s %s <RRN>\t- print a single tweet using its ID (RRN)\n", CMD_REQUEST, CMD_REQUEST_RRN);
	printf("%s <fileName>\t\t- change the data file to the specified one\n", CMD_CHANGE_DATAFILE);
	printf("%s\t\t\t- list all files in current directory\n", CMD_LS);
	printf("%s\t\t\t- change the current directory\n", CMD_CD);
	printf("%s <path>\t\t- create a new directory\n", CMD_MKDIR);
	printf("%s\t\t\t- print the path of the current directory\n", CMD_PWD);
	printf("%s\t\t\t- clear the screen \n", CMD_CLEAR);
	printf("\n");
	printf("Further documentation can be found at: \n");
	printf("%s\n", DOC_PDF);
	printf("---------------------------------------------------\n");

}

void cmdMkDir(){
	char path[FILENAME_MAX];
	scanf("%s", path);
	MKDIRECTORY(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

}

/**
 * A function that changes the currently in use file
 *
 * @param fileName - the name of the currently in use file
 *
 */
void cmdCF(char *fileName){
	char fileN[FILENAME_MAX];
	FILE *file;
	printf("Currently open file: %s\n", fileName); 
	printf("Type a new filename to open or press enter to keep current one (it will be created if it does not exist):\n");
	fileN[0] = 0;
	WORKINGDIRECTORY(fileN, sizeof fileN);
	strcat(fileN, SEP);
	scanf("%s", &fileN[strlen(fileN)]);
	file = fopen(fileName, "a+");
	if (file == NULL){
		fprintf(stderr, "A problem was encountered when trying to open the file, will keep current one. Maybe check your permissions\n");
	}else{
		fclose(file);
		if (strcmp(fileN, "") != 0){
			strcpy(fileName, fileN);
		}
	}
}


/**
 * A function that prints the current working directory of the running application
 */
void cmdPWD(){
	char buf[FILENAME_MAX];
	printf("%s\n",WORKINGDIRECTORY(buf, sizeof buf));
}

/**
 * A function that changes the current working directory of the running application
 */
void cmdCd(){
	
	char path[FILENAME_MAX];
	scanf("%s", path);

	CHANGEDIR(path);
}

/**
 * A function that calls the OS file listing application
 */
void cmdLs(){
	system(LS);
}

/**
 * A function that calls the OS screen cleaning tool
 */
void cmdClear(){
	system(CLEAR);
}

/**
 * The main application function
 *
 * @param argc - the number of command line arguments
 * @param argv - the command line arguments
 *
 * @variable cmd - the command typed by the user
 * @variable buf - a temporary buffer for general use
 * @variable fileName - contains the name of the currently in use file
 * @variable file - used only to "touch" the file with fileName and check if its possible to open/create a file
 *
 */
int main(int argc, char *argv[]){

	char cmd[CMD_LENGTH], buf[CMD_LENGTH * 10];
	char fileName[FILENAME_MAX];
	FILE *file;

	fileName[0] = 0;
	strcat(fileName, WORKINGDIRECTORY(buf, sizeof buf));
	strcat(fileName, SEP);
	// if it was not passed as a command line argument, ask for the name of the file to work with
	if (argc != 2){
		printf("please input the name of a file to work with. it will be created if it does not exist:\n");
		scanf("%s", buf);
	}else{ // if it was, just put it in the fileName variable
		strcpy(buf, argv[FILENAME]);
	}
	strcat(fileName, buf);
	file = fopen(fileName, "a+"); // try to open the file for appending, creating it if needed (used to check if its possible to read/create files)
	if (file == NULL){ // if its not possible, print an error and exit
		fprintf(stderr, "A problem was encountered when trying to open a file, maybe check your permissions\n");
		exit(1);
	}
	fclose(file); // close the file, as we don't need it anymore

	printf("~(%s)~> ", WORKINGDIRECTORY(buf, sizeof buf)); // a simple prompt with current working directory to wait for a command

	// read commands until EOF is reached (or the command typed is "exit"
	while(scanf("%s", cmd) != EOF){
		// a "switch" type structure to call a function to handle each possible command
		if(strcmp(cmd, CMD_INSERT) == 0){
			cmdInsert(fileName);
		}else if(strcmp(cmd, CMD_REMOVE) == 0){
			cmdRemove(fileName);
		}else if(strcmp(cmd, CMD_REQUEST) == 0){
			cmdRequest(fileName);
		}else if(strcmp(cmd, CMD_CREDITS) == 0){
			cmdCredits();
		}else if(strcmp(cmd, CMD_CHANGE_DATAFILE) == 0){
			cmdCF(fileName);
		}else if(strcmp(cmd, CMD_CLEAR) == 0){
			cmdClear();
		}else if(strcmp(cmd, CMD_PWD) == 0){
			cmdPWD();
		}else if(strcmp(cmd, CMD_MKDIR) == 0){
			cmdMkDir();
		}else if(strcmp(cmd, CMD_CD) == 0){
			cmdCd();
		}else if(strcmp(cmd, CMD_LS) == 0){
			cmdLs();
		}else if(strcmp(cmd, CMD_HELP) == 0){
			cmdHelp(argv[PROGNAME]);
		}else if(strcmp(cmd, CMD_EXIT) == 0){
			break; // exit the program
		}else{
			fgets(buf, sizeof buf, stdin); // if the command is invalid, consume the stdin buffer
			buf[strlen(buf)-1] = 0; // as fgets adds the \n to the buffer, we remove it
			printf("Invalid command: %s%s, try typing '%s' for help\n", cmd, buf, CMD_HELP); // says that the command is not valid
		}
		printf("~(%s)~> ", WORKINGDIRECTORY(buf, sizeof buf)); //print the prompt again
	}

	return 0;
}
