// File name: WordCount.cpp
// Author:Scott Kang
// Userid: kangk
// Email: kyung.ho.kang@vanderbilt.edu
// Class: CS2201
// Assignment Number: #8
// Honor Statement: I have not given or received unauthorized aid on this assignment
// Description: By comparing words usage in each text file, we want to figure out if the authors of the texts
//              are actually one person.
// Last Changed: 11/13/2019


#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>


const double MINIMUM_APPEARANCE = 0.001;
const int STEM_LENGTH = 6;

//DO NOT CHANGE THESE
typedef std::set<std::string> WordSet;
typedef std::map<std::string, int> WordCountMap;


// function prototypes
// you are not allowed to change these function headers,
// but you can add helper functions as needed.
WordSet readCommonWords(const std::string &fname);

WordCountMap processFile(const std::string &fname, const WordSet &common);

double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname);


/* main */
int main() {
    std::string commonFile = "texts/common.txt";
    std::string fin1, fin2, fout, str;

    do {
        std::cout << "Enter name of the first input file: ";
        std::cin >> fin1;
        fin1 = "texts/" + fin1;

        std::cout << "Enter name of the second input file: ";
        std::cin >> fin2;
        fin2 = "texts/" + fin2;

        std::cout << "Enter name of the output file: ";
        std::cin >> fout;
        fout = "texts/" + fout;

        WordSet common = readCommonWords(commonFile);

        WordCountMap first = processFile(fin1, common);
        WordCountMap second = processFile(fin2, common);

        double dist = compareTexts(first, second, fout);

        std::cout << "Vector Distance: " << dist << std::endl;

        std::cout << "Would you like to run the program again (Y|N): ";
        std::cin >> str;
    } while (tolower(str[0]) == 'y');


    return 0;
}

/* readCommonWords
 * This method reads words from a given file and places them
 * into a WordSet, which it returns.
 *
 * pre:  a valid file name with 1 word per line, words all in lower case
 * post: all words int the file are placed in the WordSet
 */
WordSet readCommonWords(const std::string &fname) {

    WordSet ret;

    std::string str;
    std::string word;

    std::ifstream file(fname.c_str());

    if (!file) {
        std::cout << "Unable to open file. Press enter to exit program!.";
        getline(std::cin, str);  // consume existing line
        std::cin.get();  // get the key press
        exit(1);
    }

    while(file >> word){
        ret.insert(word);
    }

    return ret;
}

/* processFile
 * This function reads in all words from the given file
 * after reading a word it converts it to lower case,
 * removes non alphabetic characters and stems it to STEM_LENGTH.
 * If the resulting word is considered common it is ignored.
 * Otherwise, the count in the map that matches the word
 * is incremented.
 *
 * pre:  the name of a text file and a set of words to be ignored.
 * post: The file has been read; a map of cleansed words is created and returned
 *
 */
WordCountMap processFile(const std::string &fname, const WordSet &common) {

    WordCountMap ret;

    std:: string str;
    std:: string word;

    std::ifstream file(fname.c_str());

    if (!file) {
        std::cout << "Unable to open file. Press enter to exit program.";
        getline(std::cin, str);  // consume existing line
        std::cin.get();  // get the key press
        exit(1);
    }

    while(file >> word){

        std:: string newWord;

        // change the word from the file into a newWord that meets the requirements
        // the characters should be all lowercase alphabet

        for(size_t i = 0; i < word.length(); i++){
            if(word[i] >= 65 && word[i] <= 90){
                newWord += tolower(word[i]);
            }else if(word[i] >= 97 && word[i] <= 122){
                newWord += word[i];
            }
        }

        // fix the length of the word according to the STEM_LENGTH and add it to the ret map

        if(newWord.length() > STEM_LENGTH){
            newWord = newWord.substr(0, STEM_LENGTH);
        }

        if(newWord.length() != 0 && common.count(newWord) == 0){
            if(ret.count(newWord) != 0){
                ret.at(newWord) += 1;
            }else{
                ret.emplace(newWord, 1);
            }
        }

    }

    return ret;
}


/* compareTexts
 * Compares the count maps of 2 texts.
 * The result returned is the euclidean distance
 * where each word with a percentile appearance greater than
 * MINIMUM_APPEARANCE counts as a dimesion of the vectors.
 * (they still count toward the full sum used to calculate
 *  percentile appearance)
 *
 * This method also prints out the results to a file using the following
 * format (start and end tags added for clarity):
 * / -- start example output --/
 * word1:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * word2:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * ...
 * wordN:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 *
 * Counted words in document 1 = xxxx
 * Counted words in document 2 = xxxx
 *
 * Vector Distance: dist
 * /-- end example output --/
 *
 * word1-wordN are all words whose comparison is include in the final sum.
 * distance values are the square of the difference of the percentile
 *                         scores from the two maps.
 * score1 & score2 values are the scores from the two maps.
 * dist is the final distance that is also returned,
 *    it is the square root of the sum of all the individual distances.
 * To help line things up, there is a tab character after the ":" and another
 *    before the "(" on the line for each word.
 *
 * pre:  two maps of texts to be compared and the name of an output file
 * post: returns the euclidean distance of the two files
 *       if the output file cannot be opened, the method prints
 *       an error mesage and exits the program.
 */
double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname) {

    std::ofstream outfile;
    outfile.open(fname.c_str());
    if(outfile.fail()){
        exit(1);
    }

    size_t firstTotal = 0;
    size_t secondTotal = 0;


    for(auto i = first.begin(); i != first.end(); i++){
        firstTotal += i -> second;
    }

    for(auto i = second.begin(); i != second.end(); i++){
        secondTotal += i -> second;
    }

    double vectorD = 0.0;

    // use iterator to traverse through the first file

    for(auto firstIt = first.begin(); firstIt != first.end(); firstIt++){

        std:: string word = firstIt -> first;

        double scoreOne = 0.0;
        double scoreTwo = 0.0;

        if(second.count(word) > 0){
            scoreOne = (double)(*firstIt).second /firstTotal;
            scoreTwo = (double)second.at(word)/secondTotal;
        }

        // check if the score is greater than the MINIMUM_APPEARANCE value
        // if so then calculate the dist and print the score for each file as well as dist
        // add dist into vector distance

        if(scoreTwo > MINIMUM_APPEARANCE && scoreOne > MINIMUM_APPEARANCE){
            double dist = pow((scoreOne - scoreTwo),2);
            outfile << word << ":\tdistance = " << dist << "\t(score1 = " << scoreOne <<", score2 = "<< scoreTwo <<")";
            outfile << "\n";
            vectorD += dist;
        }
    }

    vectorD = sqrt(vectorD);
    outfile << "\nCounted words in document 1 = " << firstTotal << std::endl;
    outfile << "Counted words in document 2 = " << secondTotal << std::endl;
    outfile << "\nVector Distance: " << vectorD;

    outfile.close();

    return vectorD;
}
