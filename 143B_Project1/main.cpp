#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <tuple>

struct Node{
	int index;
	Node *next;
	
	Node(int i){
		index = i;
		next = nullptr;
	}
};
//special Node for resources--index now holds TUPLE instead of INT!

struct rNode{
	std::tuple<int, int> keepcount;
	rNode *next;

	rNode(int r, int k){
		keepcount = std::make_tuple(r, k);
		next = nullptr;
	}
};




class WaitLinkedList{

public:
	Node *head;
	
	//constructor
	WaitLinkedList(){
		head = nullptr;
	}
	void enqueue(int ind){
		//std::cout <<"[[[[[[[[[[ starting enqueue ]]]]]]]]]]\n";
		if (head==nullptr){
			//std::cout<<"--... head nullptr case...\n";
			head = new Node(ind);
		}
		else{
			//std::cout<<"........ NOT head nullptr case......\n";
			Node *toadd = new Node(ind);
			Node *iter = head;
			while (iter->next!=nullptr){
				iter = iter->next;
			}
			iter->next = toadd;
		}
		//std::cout<<"--------{ called enqueue )---------"<<std::endl;
		return;

	}
	void remove(int ind){
		//int found = 0;//SEPARATE FUNCTION MAYBE?
		//std::cout<<"in remove call......\n";
		Node *iter = head;
		Node *prev = nullptr;
		while(iter->next!=nullptr && iter->index !=ind)
		{
			/*if (iter->next = nullptr){
				std::cout<<"Can't remove...not in list"<<std::endl;
				error();
			}*/
			prev = iter;
			iter = iter->next;
		}
		//Node *rem = iter;

		if(head->index == ind){
			Node *ntemp = head;
			if(ntemp->next!=nullptr){
				Node *nhead = head->next;
				delete ntemp;
				head = nhead;
			}
			else{
				delete ntemp;
				head = nullptr;
			}
		}
		
		else if (iter->next==nullptr){
			//std::cout<<"Case 1:   ....\n";
			if (prev!=nullptr){
				prev->next = nullptr;
			}
			delete iter;
		}
		else{
			//std::cout<<"case 2 ....\n";
			Node *replace = iter->next;
			prev->next = replace;
			delete iter;
		}
			/*Node *rep = iter;
			Node *rem = rep->next;
			rep->index = iter->next->index;
			rep->next = iter->next->next;
			delete rem;*/
	}

	void clearall(){
		Node *niter = head;
		while(niter!=nullptr){
			Node *n = niter->next;
			remove(niter->index);
			niter = n;
		}
		return;
	}
	
};

// special linked list for RCB and pcb.resource too possibly...
class rLinkedList{
	public:
		rNode *head;

		rLinkedList(){
			head = nullptr;
		}

		void renqueue(int r, int k){
			if (head==nullptr){
				head = new rNode(r, k);
			}
			else{
				rNode *toadd = new rNode(r, k);
				rNode *riter = head;
				while(riter->next!=nullptr){
					riter = riter->next;
				}
				riter->next = toadd;
			}
			//std::cout <<"----------CALLED (RENQUEUE)----------------"<<std::endl;
			return;
		}

		//this function is for when resource needs to be DELETED from process.resource
		//or from resources waitlist
		void rremove(int r){
			rNode * iter = head;
			rNode *prev = nullptr;
			while (iter->next!=nullptr && std::get<0>(iter->keepcount)!= r){
				prev = iter;
				iter = iter->next;
			}
			if(std::get<0>(head->keepcount) == r){
				rNode *rtemp = head;
				if(rtemp->next!=nullptr){
					rNode *rhead = head->next;
					delete rtemp;
					head = rhead;
				}
				else{
					delete rtemp;
					head = nullptr;
				}
			}
			else if (iter->next==nullptr){
				if(prev!=nullptr){
					prev->next = nullptr;
				}
				delete iter;
			}
			else{
				rNode *rep = iter->next;
				prev->next = rep;
				//std::get<0>(rep->keepcount) = std::get<0>(iter->next->keepcount);
				//std::get<1>(rep->keepcount) = std::get<1>(iter->next->keepcount);
				delete iter;
			}
		}

		rNode* rfind(int ind){
			rNode* iter = head;
			while(iter!=nullptr && std::get<0>(iter->keepcount)!=ind){
				iter = iter->next;
			}
			if(iter==nullptr){
				return nullptr;
			}
			else{
				return iter;
			}
		}

		void rclearall(){
			rNode *riter = head;
			while(riter!=nullptr){
				rNode *rn = riter->next;
				rremove(std::get<0>(riter->keepcount));
				riter = rn;
			}
			return;
		}
				



		//void releaser(int r, int k)
};






class PCB
{
	public:
		int state;
		int priority;
		int parent;
		int blocked;
		WaitLinkedList * children;
		rLinkedList * resources;

		PCB(){
			state = -1; //to mark it as currently free, STATE IS -1
			children = new WaitLinkedList();
			resources = new rLinkedList();
			parent = -1;
			blocked = -1;
		}


};

class RCB{
public:
	int inventory;
	int initial;
	int state;
	rLinkedList *rll;//waitlist for processes requesting units...

	//constructor
	RCB(){
		inventory = 1;
		state = 1;//set to free
		rll = new rLinkedList();
		//rll = nullptr;
	}
};

PCB * processes = new PCB[16];
RCB * resources = new RCB[4];
WaitLinkedList *rl= new WaitLinkedList [3];
//rl[0].head = 0;
int running_process = 0;


void init(){
	processes[0].state = 1;
	processes[0].priority = 0;
	//make PROCESS 0
	//head of RL---
	/*std::cout<<"RL currently holds: \n";
	Node *tempn = rl[1].head;
	while(tempn!=nullptr){
		std::cout<<tempn->index<<", \n";
		tempn = tempn->next;
	}*/
	for (int i = 1; i<16; i++){
		processes[i].state = -1;
		processes[i].parent = -1;
		//processes[i].children = nullptr;//consider adding method to linked list to delete everything in list; call the method here!
		//processes[i].resources = nullptr;
		processes[i].resources->rclearall();
		processes[i].children->clearall();
		processes[i].blocked = -1;

	}
	for (int j = 0; j<4; j++){
		resources[j].rll->rclearall();
	}

	resources[0].inventory = 1;
	resources[0].state = 1;
	resources[0].initial = 1;
	resources[1].inventory = 1;
	resources[1].state = 1;
	resources[1].initial = 1;
	resources[2].inventory = 2;
	resources[2].state = 1;
	resources[2].initial = 2;
	resources[3].inventory = 3;
	resources[3].initial = 3;
	resources[3].state = 1;

	running_process = 0;
	// clear the ready list!!! ITERATE:
	for (int i = 1; i<3; i++){
		//std::cout<<"I is: "<<i<<std::endl;
		if(rl[i].head!=nullptr){
			Node *iter = rl[i].head;
			while (iter!=nullptr){
				//std::cout<<"~~~~~~~~ ~~~~~~~~~~~ loop again ~~~~~~~~~ ~~~~~~~~~~~~\n";
				Node *temp = iter;
				iter = iter->next;
				//std::cout<<"iter NEXT INDEX(before remove):  "<<iter->index<<std::endl;
				//std::cout<<"Removing index:  "<<temp->index<<".....";
				//iter = iter->next;
				rl[i].remove(temp->index);
				//std::cout<<"next remove (after remove):  "<<iter->index<<std::endl;
				//iter = temp;
			}
		}
	}

	
	std::cout<<"\n0 ";
	return;
}

void scheduler(){
	//std::cout <<"##########################SCHEDULER RUN################\n"<<std::endl;
	int topprocess = 0;
	for (int p = 2; p>=0; p--){
		if (rl[p].head != nullptr){
			topprocess = rl[p].head->index;
			//std::cout <<"TOP PROCESS:  "<<topprocess<<std::endl;
			break;
		}
	}
	running_process = topprocess;
	std::cout<<running_process<<" ";
	return;
}

void create(int pr){
	if (pr>0 && pr<=2){
		for (int i=1; i<16; i++){
			if (processes[i].state== -1){
				processes[i].state = 1; // 1 marks ready! 0 is  BLOCKED
				processes[i].priority = pr;
				int parpr = running_process;
				//std::cout<<"----_About to loop to get current process_-----"<<std::endl;
				/*for (int p = 2; p>=0; p--){
					if (rl[p].head!=nullptr){
						parpr = rl[p].head->index;
						break;
					}
				}*/
				//std::cout<<"Finished loop................\n";
				//std::cout<<"---> parpr = "<<parpr<<std::endl;
				processes[i].parent = parpr;
				//std::cout <<"assigned parpr into process i parent\n";
				(processes[parpr].children)->enqueue(i);
				rl[pr].enqueue(i);
				//std::cout<<"So the rl for this priority is: \n";
				//Node *n = rl[pr].head;
				/*while(n!=nullptr){
					std::cout<<n->index<<", ";
					n = n->next;
				}*/
				//std::cout<<"process "<<i <<"created"<<std::endl;

				//CALL SCHEDULER!
				scheduler();
				return;
				//break;
			}
		}
		std::cout<<"-1 ";
	}
	else{
		std::cout<<"-1 ";
	}
	//std::cout<<"______________ Entries of rl[priority]:\n";
	/*Node *n = rl[pr].head;
	while (n!=nullptr){
		std::cout<<n->index<<", ";
		n = n->next;
	}*/
	//std::cout<<"\n";
	
	return;
}



void timeout(){
	//switch contents of head of rl and end of rl
	//current running process:
	//std::cout <<"IN((( TIMEOUT ))))\n";
	int running = 0;
	for (int p = 2; p>=0; p--){
		if (rl[p].head != nullptr){
			if(rl[p].head->next==nullptr){
				//nothing changes
				running = rl[p].head->index;
				break;
			}
			else{//iterate through this rl index to get the last node and switch content
				int headprocess = rl[p].head->index;// delete the head then ins
				//rl[p].remove(headprocess);//delete from front!
				rl[p].remove(headprocess);
				rl[p].enqueue(headprocess);//insert at back!
				//rl[p].remove(headprocess);
				running = rl[p].head->index;
				break;
			}
		}
	}
	//std::cout<<"process "<<running<<" running...\n";
	running_process = running;
	//std::cout<<"Calling scheduler??????"<<std::endl;
	scheduler();
}


void request(int r, int k){
	//std::cout<<"CALLED REQUEST........!!\n";
	if (r>=0 && r<=3 && running_process!=0){
		//std::cout<<"passed first check...\n";
		//std::cout<<"Leftover inventory: "<<resources[r].inventory<<std::endl;
		//std::cout<<"Current state of resource: "<<resources[r].state<<std::endl;

		if(k <= resources[r].inventory && resources[r].state==1){
			//std::cout<<"into next check...\n";
			//add resource to processes[current].resources
			//processes[running_process].resources->renqueue(r, k);
			//iterate throgh process[currprocess].resources to check
			//if we can just increase k for r...
			//rNode * = separate function...rfind on rLinkedList 
			rNode *getrnode = processes[running_process].resources->rfind(r);
			//std::cout<<"....did get rnode\n";
			if (getrnode == nullptr){
				processes[running_process].resources->renqueue(r,k);
			}
			else{
				std::get<1>(getrnode->keepcount)+=k;
			}
			//std::cout<<">>>>increasing inventory>>>>\n";
			resources[r].inventory-=k;
			if(resources[r].inventory==0){
				resources[r].state = 0;//all units allocates!
			}
		}
		else if(k>resources[r].initial){
			std::cout<<"-1 ";
			return;
		}
		else{//cannot satisfy request; BLOCK PROCESS!
			//remove currprocess from ready list to waitlist of resources[r]
			int prior = processes[running_process].priority;
			processes[running_process].blocked = r;
			processes[running_process].state = 0;
			//std::cout<<"priority is: "<<prior<<std::endl;
			//std::cout<<"About to remove...\n";
			//rl[prior].remove(running_process);
			//std::cout<< "Running process to remove:: "<<running_process<<std::endl;
			rl[prior].remove(running_process);
			//std::cout<< "Removed successful...\n";
			resources[r].rll->renqueue(running_process, k);
			//scheduler();
		}
		scheduler();
	}
	else{
		std::cout<<"-1 ";
	}
	//std::cout<<"finished request command....."<<std::endl;
	return;
}

int release(int r, int k){
	// remove r from resource list of process i
	// error check: k must be less than or equal to amount held by process
	
	rNode *tempnode = processes[running_process].resources->rfind(r);
	//std::cout << "got rnode...\n";
	if (tempnode!=nullptr && k<=std::get<1>(tempnode->keepcount)){
		//std::cout<<"checked node if valid...\n";
		if (k==std::get<1>(tempnode->keepcount)){
			//std::cout<<"About to remove r from process.resources...\n";
			processes[running_process].resources->rremove(r);
			//std::cout<<"removed r from process/resources...\n";
		}
		else{
			std::get<1>(tempnode->keepcount) -= k;
		}
		resources[r].inventory+=k;
		//std::cout<<"added inventory...\n";
		if(resources[r].inventory>0){
			resources[r].state = 1;
		}
		rNode *riter = resources[r].rll->head;
		while(riter!= nullptr && std::get<1>(riter->keepcount)<= resources[r].inventory){
			int remproc = std::get<0>(riter->keepcount);
			int procrq = std::get<1>(riter->keepcount);
			processes[remproc].blocked = -1;
			//resources[r].rll->rremove(remproc); //moved!
			int rprior = processes[remproc].priority;
			rl[rprior].enqueue(remproc);
		//	std::cout<<"\n rl[2] is:";
		/*	Node *nn = rl[2].head;
			while(nn!=nullptr){
				std::cout<<" ["<<nn->index<<"] ";
				nn = nn->next;
			}
			std::cout<<"\n";*/
			processes[remproc].resources->renqueue(r, procrq);
			processes[remproc].state = 1;
			resources[r].inventory-=procrq;

			riter = riter->next;
			resources[r].rll->rremove(remproc);//moved HERE
		}
		if (resources[r].inventory==0){
			resources[r].state = 0;//resource blocked
		}
		//std::cout<<"resource "<<r<<" released! \n";
		//scheduler();
	}
	else{
		std::cout<<"-1 ";
		return 1;
	}
	return 0;
}




	

			






// FINISH LATER
int destroy(int del){
	//std::cout<<"=============================call DESTROY================\n";
	if(del==0){
		std::cout<<"-1 ";
		return 1;
	}
	int check = 0;
	if(del ==running_process){
		check = 1;
	}
	else{
		Node *niter = processes[del].children->head;
		while(niter!=nullptr){
			if(niter->index==del){
				check = 1;
				break;
			}
			niter = niter->next;
		}
	}
	int tparent = processes[del].parent;
	while (tparent!= -1){
		//std::cout<<"~~~while destroy\n";
		//std::cout<<"TPARENT = "<<tparent<<std::endl;
		if(tparent==running_process){
			check = 1;
			break;
		}
		tparent = processes[tparent].parent;
	}

	Node *iternode = processes[del].children->head;
	if (check==0){
		std::cout<<"-1 ";
		return 1;
	}
	//ITERATE THROUGH DEL'S CHILDREN AND KILL THEM RECURSIVELY
	while(iternode!=nullptr){
		Node *nextiter = iternode->next;
		destroy(iternode->index);
		iternode = nextiter;
	}
	//remove process from PARENT.CHILDREN:
	int idparent = processes[del].parent;
	processes[idparent].children->remove(del);
	if (processes[del].state==0){//check if blocked...
		int wlr = processes[del].blocked;//index of resource waitlisted on
		resources[wlr].rll->rremove(del);
	}
	else{
		int pind = processes[del].priority;
		rl[pind].remove(del);
	}
	rNode *iteres = processes[del].resources->head;
	int saverunningproc = running_process;
	running_process = del;
	while(iteres!=nullptr){//iterating through the resources of del
		//release resource
		rNode *n = iteres->next;
		int resid = std::get<0>(iteres->keepcount);//ID of the resource 
		int reqid = std::get<1>(iteres->keepcount);
		release(resid, reqid);
		//if(relres==0){
		//	scheduler();
		//}
			
			//then go on to the next resource
		iteres = n;
	}
	running_process = saverunningproc;
	processes[del].state = -1;
	(processes[del].children)->clearall();
	(processes[del].resources)->rclearall();
	processes[del].parent = -1;
	//scheduler();
	//std::cout<<"Destroy called...\n";
	return 0;
}





int main(){
	//std::cout<<"Starting up.."<<std::endl;
	std::string origstr;
	while(std::getline(std::cin, origstr)){
		
			//std::cout<<origstr<<std::endl;
		//tokenizing and parsing...
			if(origstr.length()==0){
				continue;
			}
			std::vector<std::string> toks;
			std::stringstream ss(origstr);
			std::string newstr;
			while(getline(ss,newstr, ' ')){
				toks.push_back(newstr);}
			//std::cout<<"TOKS[0] is: "<<toks[0]<<std::endl;

			if(toks[0].compare("in")==0){
				//std::cout<<"INITCALL...\n"<<std::endl;
				init();
				//toks.clear()
			}
			else if(toks[0].compare("to")==0){
				//std::cout<<"====CALL TIMEOUT===="<<std::endl;
				timeout();
				//toks.clear()
			}
			else if(toks[0].compare("cr")==0){
				//std::cout<<"========Call CREATE +++++++++"<<std::endl;
				int arg = stoi(toks[1]);
				create(arg);
			}
			else if(toks[0].compare("rq")==0){
				int arga = stoi(toks[1]);
				int argb = stoi(toks[2]);
				request(arga, argb);
			}
			else if(toks[0].compare("rl")==0){
				int a = stoi(toks[1]);
				int b = stoi(toks[2]);
				int go = release(a, b);
				if(go==0){
					scheduler();
				}
			}
			else if(toks[0].compare("de")==0){
				int del = stoi(toks[1]);
				int sch = destroy(del);
				if(sch==0){
					scheduler();
				}
			}
			else{
				std::cout<<"other command\n"<<std::endl;
				//toks.clear();
			}
			/*std::cout<<"TOKENS:"<<std::endl;
			for (unsigned int i = 0; i<toks.size(); i++){
				std::cout<<"---->"<<toks[i]<<std::endl;
			}*/
			toks.clear();
		}
		//else{	
		//std::cout <<"END\n"<<std::endl;
		//continue;
		//return 0;
		return 0;
}

