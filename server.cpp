#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <vector>
#include <mutex>
#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <experimental/unordered_map>
#include <debug/unordered_map>
#include <debug/unordered_map>
#include <debug/unordered_map>

using namespace std;

mutex mtx, quizMtx;
int servFd;

unordered_set<int> clientFds;
void ctrl_c(int);
void sendQuestion(int fd, char * buffer);
uint16_t readPort(char * txt);
void setReuseAddr(int sock);
void recive(int clientFd);

/**
 * structure Quiz
 * 
 * stores the data of quiz:
 *      - accessCode - code which allows you to join the game
 *      - questionTxt[i] - text of the question[i]
 *      - answer[i] - correct answer for the question[i] 
 *      - numOfQuestions - number of questions in this quiz
 * 
 */
struct Quiz{
        int accessCode;
        unordered_map<string, char> questionWithAnswer;
        //questionTxt[10];
        //char answer[10];
        int numOfQuestions;
        int authorFd;
        int lock;
    };
    
struct quiz_ranking_struct{
        int client;
        int score;
        int quizCode;
};

/**
 *  lastQuiz stores actual number of quizes
 *  ptr_tab stores pointers to quizes
 */
int lastQuiz = 2;
Quiz ** ptr_tab = new Quiz*[lastQuiz];
int numberOfClients = 0;
quiz_ranking_struct ** client_ranking = new quiz_ranking_struct*[numberOfClients];
int size = 0;
quiz_ranking_struct * quizes;

int main(int argc, char ** argv){
    
    //----------        sample quizes            ---------------//

    ptr_tab[0] = new Quiz;
    Quiz quiz;
    quiz.accessCode = 0;
    quiz.questionWithAnswer["ILe to jest 2+2? \nA. 4 \t B. 5 \t C.3 \t D. 6"] = 'a';
    quiz.numOfQuestions = 1;
    quiz.authorFd = servFd;
    quiz.lock = 0;
    *ptr_tab[0] = quiz;
    
    Quiz quiz1;
    ptr_tab[1] = new Quiz;
    quiz1.accessCode = 1;
    quiz1.questionWithAnswer["Ile to jest 3+2? \nA. 4 \t B. 5 \t C.3 \t D. 6"] = 'b';
    quiz1.numOfQuestions = 1;
    quiz1.authorFd = servFd;
    quiz1.lock = 0;
    *ptr_tab[1] = quiz1;
    
	//---------- get and validate port number ---------------//
	if(argc != 2) error(1, 0, "Need 1 arg (port)");
	auto port = readPort(argv[1]);
	
	servFd = socket(AF_INET, SOCK_STREAM, 0);
	if(servFd == -1) 
		error(1, errno, "socket failed");
	
	signal(SIGINT, ctrl_c);
	signal(SIGPIPE, SIG_IGN);
	
	setReuseAddr(servFd);
	
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	
	res = listen(servFd, 1);
	if(res) 
		error(1, errno, "listen failed");
	
	//int num = 0;
	std::thread threads[100];
	int w = 0;
	while(true){
		sockaddr_in clientAddr{0};
		socklen_t clientAddrSize = sizeof(clientAddr);
		
		auto clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
		if(clientFd == -1) 
			error(1, errno, "accept failed");    
		
		clientFds.insert(clientFd);
		
		printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
		
		threads[w++] = std::thread(recive, clientFd);	
        
	}

    for(int i = 0; i < w; i++){
		threads[i].join();
        threads[i].detach();
    }
}

/**
 * function readPort reads the port number as txt and converts it to uint16_t
 * if pointer is null returns the error
 * 
 * @param char * txt - pointer that stores first argument of function main
 * 
 * returns port
 * 
 */
uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
		error(1,0,"illegal argument %s", txt);
	return port;
}

void setReuseAddr(int sock){
	const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res) 
		error(1,errno, "setsockopt failed");
}
/**
 * function ctrl_c closes server, program and the connections with clients 
 * and clears the memory 
 * when server was closed by combination of ctrl+c
 * 
 * @param int
 *
 */
void ctrl_c(int){
    for(int clientFd : clientFds)
        close(clientFd);
    close(servFd);
    printf("\nClosing server\n");
    for(int i=0; i<lastQuiz; i++){
        delete ptr_tab[i];
    }
	exit(0);
}

/**
 * function sendQuestion sends the message to each client which is connected  t othe server
 * 
 * @param fd - target file descriptor
 *        buffer - message from server
 *        count - individual number of client 
 * 
 */
void sendQuestion(int fd, string buff){
        
        char temp[buff.length()];
        strcpy(temp, buff.c_str());
           
        mtx.lock();
            write(fd, temp, sizeof(temp));           
        mtx.unlock();
}

/**
 * sorts the players by them score descending
 * 
 * @param   size           - size of client_ranking, exactly numberOfClients
 * @param   client_ranking - table which stores the quiz_ranking_struct structures
 * @param   clientFd       - client file descriptor used to find final position in ranking
 * @param   acces          - quiz's acces code used to distinguish between quizzes
 * 
 * returns position for a given client
 * 
 */
int sort(int size, quiz_ranking_struct ** client_ranking, int clientFd, int acces){//,quiz_ranking_struct *quizes,  ){
    quiz_ranking_struct temp;
    quiz_ranking_struct * rankWsk;
    
    int position = 0;
    int k = 0;
    quiz_ranking_struct tab[numberOfClients];
    
            if(size > 1){
                for(int i = 0; i < numberOfClients; i++){
                    
                    rankWsk = client_ranking[i];
                    
                    if(rankWsk->quizCode == acces){
                        
                           tab[k] = *client_ranking[i]; 
                           k++;
                    }
                }
                    
                for (int i = 0; i < k; i++) {
                    for (int j = i + 1; j < k; j++) {
                       
                            if (tab[i].score < tab[j].score) {
                                temp = tab[i];
                                tab[i] = tab[j];
                                tab[j] = temp;
                            }
                    }
                }
                for (int i = 0; i < k; i++) { 
                    if(tab[i].client == clientFd){
                        position = i+1;
                    }
                }
            }
            else{
                position = 1;   
            }
    
    return position;
}

/**
 * function addClientToRanking adds a new client to the quiz ranking
 * 
 * @param   clientFd - client's file descriptor 
 * @param   acces    - access code to the quiz 
 * @param   score    - score gained in the quiz
 * 
 */
void addClientToRanking(int clientFd, int acces, int score){
    
    quiz_ranking_struct **new_cl = new quiz_ranking_struct* [numberOfClients+1]; 

    for (int i = 0; i < numberOfClients; i++) {
        new_cl[i] = client_ranking[i];
    }
                                
    new_cl[numberOfClients] = new quiz_ranking_struct;
    quiz_ranking_struct newie;
    
    newie.client = clientFd;    
    newie.score = score;
    newie.quizCode = acces;
    
    *new_cl[numberOfClients] = newie;
    delete [] client_ranking;
    client_ranking = new_cl;
    
    numberOfClients++;
}


/*
 * function returnString
 * 
 * @param buffer - message from client
 *        start - index from which algorithm has to start reading until the special sign - "*"
 * 
 * returns part of buffer - string which is a :
 *      - text of the question or
 *      - given answer for question 
 * 
 */
string returnString(char *buffer, int start){
    
    string str = "";
    
    while(buffer[start] != '*') {
            str += buffer[start];
            start++;
    }

    return str; 
}
/**
 * function returnCode
 * 
 * @param   sep     - char used to separete data (code and number of question) in one message
 * @param   buffer  - pointer to the content of message from client
 * 
 * returns the accces code to the quiz
 */
int returnCode(char sep, char * buffer){
        char *str = buffer;
        int acces = atoi(strtok_r(str, &sep, &str));
                
   return acces;              
}

/**
 * function checkCode
 * 
 * @param   acces - access code to quiz
 *
 * returns -1 when code is not in the table
 *          i when code is in the table on position i
 */
int checkCode(int acces){
    int index = -1;
    Quiz *quiz = nullptr;
                
    for(int i=0; i<lastQuiz; i++){ 

        quiz = ptr_tab[i];
        if(acces == quiz->accessCode){
            index = i;
            break;
        }
    }
    
    return index;
}
/**
 * function recive
 * 
 * @param clientFd - client's file descriptor
 * 
 * reads the message from the client and depending on the sign readed from the message 
 * either joins the quiz or creates a quiz
 * 
 * clients_answers  - number of correct answers given by client
 * buffer           - message from client
 * acces            - access code to quiz
 * index            - index of the joining quiz  
 * 
 * questionNew      - the content of new question 
 * answerNew        - the correct answer of new question
 * accesNew         - access code of new quiz
 * capNew           - number of questions in new quiz
 */
void recive(int clientFd)
{
	while(1)
	{
		char buffer[255];
        int index;
        int acces;
		int count = read(clientFd, buffer, 255);
        string message = "";
        time_t start;

		if(count < 1) {
			printf("removing %d\n", clientFd);
			clientFds.erase(clientFd);
			close(clientFd);
			break;
        }
		
		//if start button was clicked
		char sign = buffer[0];
            if(sign == '@'){
                
                acces = returnCode('@', buffer);
                index = checkCode(acces);
                 if(index == -1){
                
                     message = "Nie ma takiego quizu.";
                     sendQuestion(clientFd, message); 
                }
                else{
                    Quiz *quiz = ptr_tab[index];
                    if(clientFd == quiz->authorFd || quiz->accessCode == 0 ||  quiz->accessCode == 1){ 
                            
                        //zablokuj dostęp do quizu, aż do skończenia quizu
                        start = clock();
                        quiz->lock = 0;
                       
                        while(start < 10000000){
                        
                            start = clock()-start;
                        }
                        quiz->lock = 1;                       
                        string txt = "!#";
                        sendQuestion(clientFd, txt);
                    } 
                    else{ 
                    
                     message = "Nie możesz rozpocząć cudzego quizu.";
                     sendQuestion(clientFd, message); 
                    }
                
                }
            }
            //if join button was clicked
            if(sign == '&'){
                
                acces = returnCode('&', buffer);
                index = checkCode(acces);
                if(index == -1){
                
                     message = "!*";
                     sendQuestion(clientFd, message); 
                }
                else{
                    Quiz *quiz = ptr_tab[index];
                        
                    if(clientFd == quiz->authorFd){ //ptr_tab[index]
                            
                        message = "Nie możesz wziąć udziału we własnym quizie.";
                        sendQuestion(clientFd, message);   
                        
                    } 
                    else{ 
                        
                        if(quiz->lock == 0){
                            
                            int clients_answers = 0;
                            int client_score = 0;
                            index = -1; 
                                                                
                            start=clock();
                            addClientToRanking(clientFd, acces, client_score);
                            
                            for(int i = 0; i < numberOfClients; i++){
                                    
                                if(client_ranking[i]->client == clientFd){
                                
                                    index = i;
                                    break;
                                }
                            }
                    //---------- send question and read the answer -----------------//            
                            for(auto quest : quiz->questionWithAnswer){
                                                        
                                sendQuestion(clientFd, quest.first);
                                                                    
                    //------------------ wait for the answer -----------------------//
                                
                                char bufferA[255];
                                    
                                while(bufferA[0] != '?'){
                                    
                                    read(clientFd, bufferA, sizeof(bufferA));
                                }
                                    
                                    start = clock()-start;
                                            
                                    char *str1 = bufferA;
                                    char *answerUser = strtok_r(str1, "?", &str1);
                                        
                    //----------- compare the answer with the correct one -----------//
                                    if(quest.second == *answerUser){
                                                
                                        clients_answers++;
                                                
                                        if(((float)start/CLOCKS_PER_SEC) > 0.001)
                                        {
                                            client_score += 100;
                                        }
                                        else{
                                            client_score += 0.1/((float)start/CLOCKS_PER_SEC);
                                        }
                                    }    
                            }
                            
                            client_ranking[index]->score = client_score;
                            
                            while(quiz->lock != 1){
                            
                                //wait for the end of quiz
                            }
                            
                            int miejsce = sort(numberOfClients, client_ranking, clientFd, acces);
                         
                            //send the result sumup
                            string finalResult = "Twój wynik: "+to_string(client_score) 
                                                + "\n Udzieliłeś "+ to_string(clients_answers) + "/" 
                                                + to_string(quiz->numOfQuestions) + " poprawnych odpowiedzi.\n" 
                                                + "Zająłeś " + to_string(miejsce) + " miejsce. Gratulacje!";
                            
                            sendQuestion(clientFd, finalResult); 
                          
                        } 
                        else{
                         
                            message = "Quiz już się rozpoczął. Nie można dołączyć!";
                            sendQuestion(clientFd, message);
                        }
                        
                    } 
                }
        }
    //----------- if create a new quiz button was clicked -----------//
            else if(sign == '#'){
                
                int znaleziono = 0;
                int counterPrev = 0;
                int accesNew = 0;
                int capNew = 0; 
                char *str = buffer;
                char *message;     
                
                while ((message = strtok_r(str, "#", &str))) { 
                           
                    if(counterPrev == 0){
                                
                        accesNew = atoi(message);   
                        counterPrev++;
                    }
                    else{
                                
                        capNew = atoi(message);
                        counterPrev=0;     
                    }
                }
                
                Quiz *quiz = nullptr;
                
                //find if code is already used        
                for(int i = 0; i < lastQuiz; i++){
                    quiz = ptr_tab[i] ;
                    if(quiz->accessCode == accesNew){
                        
                        string message = "!?\n";
                        sendQuestion(clientFd, message);
                        znaleziono++;
                        break;
                    }
                }
                
                if(!znaleziono){

                    Quiz **new_tab = new Quiz* [lastQuiz+1]; 

                    for (int i = 0; i < lastQuiz; i++) {
                        new_tab[i] = ptr_tab[i];
                    }
                    
                    new_tab[lastQuiz] = new Quiz;
                    
                    Quiz quiz1;
                    quiz1.numOfQuestions = capNew;
                    quiz1.authorFd = clientFd;
                    quiz1.accessCode = accesNew;
                    quiz1.lock = 1;
                    
                    //------------- read and save questions and answers -------------//
         
                    for(int i=0; i<capNew; i++){
                        mtx.lock();       
                        
                        count = read(clientFd, buffer, 255);
                        string questionNew = returnString(buffer, 0);
                        string buf = buffer;
                        int sepLoc = buf.find("*");
                        char answerNew = returnString(buffer, sepLoc-1)[0];
                        
                        quiz1.questionWithAnswer[questionNew] = answerNew; 
                        mtx.unlock();
                        
                    }
                    
                    *new_tab[lastQuiz] = quiz1;
                    delete [] ptr_tab;
                    ptr_tab = new_tab;
                    lastQuiz += 1;
     
                }
            }
	}
}

