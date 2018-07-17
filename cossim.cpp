#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const int MAX_STOPWORDS = 200;
const int MAX_TOKENS = 400;
const char* DEFAULT_STOPWORDS_FILENAME = "stopwords.txt";
const char* CHARS_TO_REMOVE = ".,:;\"()";

std::string readInputFile(const char* filename) {
  std::string text;
  std::ifstream file(filename);
  std::getline(file, text);
  file.close();
  return text;
}

std::string* readStopwordsFile(const char* filename, int& size) {
  std::string* stopwords = new std::string[MAX_STOPWORDS];
  std::string stopword;
  std::ifstream file(filename);
  size = 0;
  while (std::getline(file, stopword) && size < MAX_STOPWORDS) {
    stopwords[size++] = stopword;
  }
  file.close();
  return stopwords;
}

void preprocess(std::string& text) {
  for (int i = 0; CHARS_TO_REMOVE[i]; ++i) {
    text.erase(
      std::remove(text.begin(), text.end(), CHARS_TO_REMOVE[i]),
      text.end());
  }
  std::transform(text.begin(), text.end(), text.begin(), tolower);
}

std::string* tokenize(std::string text, int& size) {
  std::string* tokens = new std::string[MAX_TOKENS];
  std::stringstream stream(text);
  size = 0;
  while (stream.good() && size < MAX_TOKENS) {
    stream >> tokens[size++];
  }
  return tokens;
}

bool inArray(std::string str, std::string* strArray, int strArraySize) {
  for (int i = 0; i < strArraySize; ++i) {
    if (str == strArray[i]) {
      return true;
    }
  }
  return false;
}

std::string* removeStopwords(std::string* tokens, int tokensSize,
                             std::string* stopwords, int stopwordsSize,
                             int& size) {
  std::string* tokensNoStopwords = new std::string[tokensSize];
  size = 0;
  for (int i = 0; i < tokensSize; ++i) {
    if (!inArray(tokens[i], stopwords, stopwordsSize)) {
      tokensNoStopwords[size++] = tokens[i];
    }
  }
  return tokensNoStopwords;
}

int frequency(std::string token, std::string* tokens, int tokensSize) {
  int freq = 0;
  for (int i = 0; i < tokensSize; ++i) {
    if (token == tokens[i]) {
      ++freq;
    }
  }
  return freq;
}

int dotProduct(int* v1, int* v2, int size) {
  int prod = 0;
  for (int i = 0; i < size; ++i) {
    prod += v1[i] * v2[i];
  }
  return prod;
}

double magnitude(int* v, int size) {
  return std::sqrt(dotProduct(v, v, size));
}

double cosineSimilarity(std::string* tokens1, int tokens1Size,
                        std::string* tokens2, int tokens2Size) {
  std::string* distinctTokens = new std::string[tokens1Size + tokens2Size];
  int distinctTokensSize = 0;
  for (int i = 0; i < tokens1Size; ++i) {
    if (!inArray(tokens1[i], distinctTokens, distinctTokensSize)) {
      distinctTokens[distinctTokensSize++] = tokens1[i];
    }
  }
  for (int i = 0; i < tokens2Size; ++i) {
    if (!inArray(tokens2[i], distinctTokens, distinctTokensSize)) {
      distinctTokens[distinctTokensSize++] = tokens2[i];
    }
  }
  int* frequencies1 = new int[distinctTokensSize];
  int* frequencies2 = new int[distinctTokensSize];
  for (int i = 0; i < distinctTokensSize; ++i) {
    frequencies1[i] = frequency(distinctTokens[i], tokens1, tokens1Size);
    frequencies2[i] = frequency(distinctTokens[i], tokens2, tokens2Size);
  }
  delete[] distinctTokens;
  double sim = dotProduct(
    frequencies1, frequencies2, distinctTokensSize) /
    (magnitude(frequencies1, distinctTokensSize) *
    magnitude(frequencies2, distinctTokensSize));
  delete[] frequencies1;
  delete[] frequencies2;
  return sim;
}

void writeResult(const char* filename, double result) {
  std::ofstream file(filename);
  file << result << std::endl;
  file.close();
}

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout <<
      "Usage:" << std::endl <<
      "  cossim <input1> <input2> <output> [<stopwords>]" << std::endl;
    return 1;
  }
  std::string text1 = readInputFile(argv[1]);
  std::string text2 = readInputFile(argv[2]);
  int stopwordsSize;
  std::string* stopwords = readStopwordsFile(
    argc > 4 ? argv[4] : DEFAULT_STOPWORDS_FILENAME, stopwordsSize);
  preprocess(text1);
  preprocess(text2);
  int tokens1Size;
  std::string* tokens1 = tokenize(text1, tokens1Size);
  int tokens2Size;
  std::string* tokens2 = tokenize(text2, tokens2Size);
  int tokensNoStopwords1Size;
  std::string* tokensNoStopwords1 = removeStopwords(
    tokens1, tokens1Size, stopwords, stopwordsSize, tokensNoStopwords1Size);
  delete[] tokens1;
  int tokensNoStopwords2Size;
  std::string* tokensNoStopwords2 = removeStopwords(
    tokens2, tokens2Size, stopwords, stopwordsSize, tokensNoStopwords2Size);
  delete[] tokens2;
  delete[] stopwords;
  double similarity = cosineSimilarity(
    tokensNoStopwords1, tokensNoStopwords1Size,
    tokensNoStopwords2, tokensNoStopwords2Size);
  delete[] tokensNoStopwords1;
  delete[] tokensNoStopwords2;
  writeResult(argv[3], similarity);
  return 0;
}
