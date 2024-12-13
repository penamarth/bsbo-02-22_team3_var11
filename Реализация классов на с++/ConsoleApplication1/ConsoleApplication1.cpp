#include <iostream>
#include <vector>
#include <string>
#include <stack>

using namespace std;

class File;
class Command;
class addCommand;
class delCommand;
class Editor;
class Paragraph;
class association;


class Command {

public:

    virtual bool execute() = 0;
    virtual bool undo() = 0;
    //virtual shared_ptr<Command> cr_undo_comm() = 0;

};

class Paragraph {

    string* str;
    friend class File;

public:

    Paragraph(string* str) { this->str = str; }

    bool del(string str, int pos) {
        cout << "   Paragraph : del(\"" + str + "\", " + to_string(pos) + ")\n";
        cout << "   // Old string: \"" + *this->str + "\")\n";
        try {
            this->str->erase(pos, str.size());
            cout << "   // New string: \"" + *this->str + "\" \n";
            //cout << "   Paragraph: return result\n" << "  File: return result\n" << " Command: return result\n";
            return 0;
        }
        catch (string massege) {
            cout << "   Error in Paragraph del()\n";
            // cout << "   Paragraph: return result\n" << "  File: return result\n" << " Command: return result\n";
            return 1;
        }


    }

    bool add(string str, int pos) {

        cout << "   Paragraph : add(\"" + str + "\", " + to_string(pos) + ")\n";

        if (pos >= 0 && pos < this->str->length()) {

            cout << "    // Old string: \"" + *this->str + "\" \n";

            this->str->insert(pos, str);

            cout << "    // New string: \"" + *this->str + "\" \n";
            //   cout << "   Paragraph: return result\n" << "  File: return result\n" << " Command: return result\n";
            return 1;

        }

        else if (pos >= this->str->length()) {

            cout << "    // Old string: \"" + *this->str + "\" \n";

            this->str->append(str);

            cout << "    // New string: \"" + *this->str + "\" \n";
            //  cout << "   Paragraph: return result\n" << "  File: return result\n" << " Command: return result\n";

            return 1;
        }

        //cout << "   Paragraph: return result (error)\n" << "  File: return result\n" << " Command: return result\n";

        return 0;

    }
};


class File {

    vector <Paragraph*> paragraphs;
    vector <shared_ptr< Command>> commandHistory;
    int currCommand;
    friend class Editor;



    int findParagraph(int* position) {

        cout << "  File: findParagraph(" + to_string(*position) + ")\n ";

        int paragraph = 0;
        while (*position >= paragraphs[paragraph]->str->size()) {
            *position -= paragraphs[paragraph]->str->size();
            paragraph++;
            if (paragraph == paragraphs.size()) {
                paragraph--;
                *position = paragraphs[paragraph]->str->size();
                break;
            }
        }

        return paragraph;

    }

public:
    bool new_command = 1;
    int currPosition;
    int currPh;
    //int currPh;

    File(vector <Paragraph*> p) {
        paragraphs = p;
        currCommand = 0;
        currPosition = 0;
        currPh = 0;
    }

    void undo() {

        if (currCommand >= 0) {

            (commandHistory[currCommand])->undo();

            //commandHistory.push_back(commandHistory[currCommand]->cr_undo_comm());

            /*if (typeid(commandHistory[currCommand]) == typeid(addCommand()))
            {
                cout <<"@@@@@@\n";
            }
            else if (typeid(commandHistory[currCommand]) == typeid(delCommand()))
            {
                cout << "$$$$$$$$$\n";
            }
            else cout << "error in undo with typeid\n ";*/


            currCommand--;
        }
    }
    void redo() {

        if (currCommand + 1 < commandHistory.size()) {
            commandHistory[++currCommand]->execute();
            commandHistory.push_back(commandHistory[currCommand]);
            //currCommand++;
        }
        else
            cout << "error in redo\n";

    }

    bool add(string str) {
        if (new_command) {

            currCommand = commandHistory.size();
        }

        cout << "  File: add(\"" + str + "\", " + to_string(currPosition) + ")\n";
        // int pos = currPosition;
        // int paragraph = findParagraph(&pos);

        // cout << "   // Insurt into paragraph nomber: " + (to_string(currPh + 1)) + ", end position: " + to_string(currPosition) + "\n";

        return this->paragraphs[currPh]->add(str, currPosition);

    }

    bool del(string str) {
        if (new_command)
            currCommand = commandHistory.size();
        cout << "  File: del(\"" + str + "\", " + to_string(currPosition) + ")\n";
        //int pos = currPosition;
        //int paragraph = findParagraph(&pos);
        return this->paragraphs[currPh]->del(str, currPosition);

    }

    void saveCommand(shared_ptr< Command> c) {
        cout << "  File: saveCommand()\n";
        commandHistory.push_back(c);
    }

};


class delCommand :public Command {
    File* file;

    string str;
    int position = 0;

    bool is = 0;
    //friend class File;

public:
    delCommand(string str, File* file) {

        cout << " delCommand: create(\"" + str + "\", file)\n";

        this->str = str;
        this->position = file->currPosition;
        this->file = file;
        is = 1;

    }

    bool execute() override {

        if (is) {

            cout << " delCommand: execute()\n";

            if (file != nullptr) {
                //file->saveCommand(this);
                return this->file->del(str);
            }
        }

        cout << " delCommand: return result (error)\n";


        return 0;

    }

    bool undo() override {
        cout << " delCommand: undo()\n";
        if (file != nullptr) {
            //file->saveCommand(this);
            return this->file->add(str);
        }
        return 1;
    }
    /*shared_ptr<Command> cr_undo_comm() override {
        shared_ptr< Command> command{ new addCommand(str, file) };
        return command;
    }*/
};

class addCommand :public Command {

    File* file;

    string str;
    int position = 0;


    bool is = 0;

public:
    addCommand(string str, File* file) {

        cout << " addCommand: create(\"" + str + ", file)\n";

        this->str = str;
        //this->paragraph = paragraph;
        this->position = file->currPosition;
        this->file = file;

        is = 1;

    }
    bool execute() override {

        if (is) {

            cout << " addCommand: execute()\n";

            if (file != nullptr) {
                // file->saveCommand(this);
                return this->file->add(str);
            }
        }

        cout << " addCommand: return result (error)\n";

        return 0;

    }

    bool undo() override {
        cout << " addCommand: undo()\n";
        if (file != nullptr) {

            return this->file->del(str);
        }

        return 1;
    }


    /* shared_ptr<Command> cr_undo_comm() override {
         shared_ptr< Command> command{ new delCommand(str, file) };
         return command;
     }*/
};

//class association {
//public:
//    vector<shared_ptr< Command>> docom;
//    vector<shared_ptr< Command>> uncom;
//
//    association(vector<shared_ptr< Command>> d, vector<shared_ptr< Command>> u) {
//        docom = d;
//        uncom = u;
//    }
//
//};

class Editor {

    File* file;

    vector<shared_ptr< Command>> comm;

    int pos = 0;
public:

    Editor(File* file) {

        this->file = file;

    }

    void setCurs(int position) {

        cout << "Editor: setCurs(" + to_string(position) + ")\n";
        pos = position;

        file->currPh = file->findParagraph(&position);
        file->currPosition = position;

    }

    void addstring(string str) {

        cout << "Editor: addstring(\"" + str + "\")\n";
        shared_ptr< Command> command{ new addCommand(str, file) };
        file->new_command = 1;

        if (command->execute()) {
            file->saveCommand(command);
            comm.push_back(command);
          //  setCurs(pos + str.length());
        }

    }

    void delstring(string str) {
        file->new_command = 1;
        cout << "Editor: delstring(\"" + str + "\")\n";
        shared_ptr<Command> command{ new delCommand(str, file) };

        if (!command->execute()) {
            file->saveCommand(command);
            comm.push_back(command);
        }
        //setCurs(str.length()-pos);
    }

    void undo() {
        file->new_command = 0;
        cout << "Editor: undo()\n";
        file->undo();
    }

    void redo() {
        file->new_command = 0;
        cout << "Editor: redo()\n";
        file->redo();
    }
};


int main()
{
    string string1 = "Hello, World!";
    string string2 = "Mey, mey!";
    string string3 = "Raf, raf!";
    string string4 = "0";

    Paragraph paragraph1(&string1);
    Paragraph paragraph2(&string2);
    Paragraph paragraph3(&string3);
    Paragraph paragraph4(&string4);

    File file({ &paragraph1, &paragraph2, &paragraph3,&paragraph4 });

    Editor editor(&file);

    /* editor.setCurs(1,0);
     editor.addstring("Hi, I'm cat, I say: ");
     cout << "\n";

     editor.setCurs(2, 11);
     editor.addstring(" - seys dog.");
     editor.showLastEdit();*/

     //editor.setCurs(70);
     //editor.addstring("Where error?");


     //Хороший пример
     editor.setCurs(5);
     editor.delstring(", World");
     editor.undo();
     editor.redo();


   // //editor.setCurs(50);
   // editor.addstring("1");
   // //editor.addstring("2");
   //// editor.addstring("3");
   // editor.addstring("2");
   // editor.undo();
   // editor.addstring("3");
   // editor.undo();
   // editor.undo();


}

