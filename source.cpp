#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <iomanip>

using namespace std;

class Movie
{
    private:
        int movieID, pubYear;
        string movieName;
        vector<int> starVec = {0, 0, 0, 0, 0};
        int numReviews = 0;
    public:
        Movie(int movieid, string name, int year)
        : movieID(movieid), movieName(name), pubYear(year){}

        //public getter functions
        int getMovieID(){return movieID;}
        int getPubYear(){return pubYear;}
        string getMovieName(){return movieName;}
        int getNumReviews(){return numReviews;}

    void setReviews(int val)
    {
        starVec.at(val-1) = starVec.at(val-1) + val;
        numReviews++;
    }

    float calcAvgRating()
    {
        int sumRatingVal = 0;
        float avgRating;
        for(auto s: starVec){
            sumRatingVal += s;
        }

        if(sumRatingVal == 0)
            avgRating = 0;
        else
            avgRating = double(sumRatingVal)/numReviews;

        return avgRating;
    }

    void printInformation()
    {
        cout << endl;
        cout << "Movie:      '" << getMovieName() << "'" << endl;
        cout << "Year:        " << getPubYear() << endl;
        cout << "Avg rating:  " << calcAvgRating() << endl;
        cout << "Num reviews: " << getNumReviews() << endl;
        cout << " 1 star:     " << starVec.at(0)/1 << endl;
        cout << " 2 stars:    " << starVec.at(1)/2 << endl;
        cout << " 3 stars:    " << starVec.at(2)/3 << endl;
        cout << " 4 stars:    " << starVec.at(3)/4 << endl;
        cout << " 5 stars:    " << starVec.at(4)/5 << endl;
        cout << endl;
    }
};

class Review
{
    private:
        int reviewID, movieID, userID, rating;
        string reviewDate;
    public:
        Review(int reviewid, int movieid, int userid, int r, string date)
        : reviewID(reviewid), movieID(movieid), userID(userid), rating(r), reviewDate(date){}

    void printInformation(map<int,Movie> moviesMap)
    {
        auto iter = moviesMap.find(movieID);
        cout << endl;
        cout << "Movie: " << movieID << " (" << iter->second.getMovieName()<< ")" << endl;
        cout << "Num stars: " << rating << endl;
        cout << "User id:   " << userID << endl;
        cout << "Date:      " << reviewDate << endl;
        cout << endl;
    }

};

map<int, Movie> inputMovies(string moviesFN)
{
  map<int, Movie> movieMap;
  string movieID, pubYear, movieName;

  ifstream movieFile(moviesFN);
  if(!movieFile.good())
  {
      cout << "Error: movies.csv won't open" << endl;
      exit(-1);
  }

  getline(movieFile, moviesFN);  //skip the first line;
  while(getline(movieFile, moviesFN))
  {
      stringstream ss(moviesFN);

      getline(ss, movieID, ',');
      getline(ss, movieName, ',');
      getline(ss, pubYear);

      Movie M(stoi(movieID), (movieName), stoi(pubYear));
      movieMap.emplace(pair<int, Movie>(stoi(movieID),M)); //store Movie object and ID into the map
  }
  movieFile.close();

  return movieMap;
}


map<int, Review> inputReviews(string reviewsFN, map<int, Movie>& moviesMap)
{
  map<int, Review> reviewsMap;
  string reviewID, movieID, userID, rating, reviewDate;

  ifstream reviewFile(reviewsFN);
  if(!reviewFile.good())
  {
      cout << "Error: reviews.csv won't open" << endl;
      exit(-1);
  }

  getline(reviewFile, reviewsFN);   //skip the first line;
  while(getline(reviewFile, reviewsFN))
  {
      stringstream ss(reviewsFN);

      getline(ss, reviewID, ',');
      getline(ss, movieID, ',');
      getline(ss, userID, ',');
      getline(ss, rating, ',');
      getline(ss, reviewDate);

      Review R(stoi(reviewID), stoi(movieID), stoi(userID), stoi(rating), reviewDate);
      reviewsMap.emplace(pair<int, Review>(stoi(reviewID), R));

      //If movieID matches then add ratings and increment numReviews
      auto iter = moviesMap.find(stoi(movieID));
      if(iter != moviesMap.end())
      {
          iter->second.setReviews(stoi(rating));
      }

  }
  reviewFile.close();

  return reviewsMap;
}


void printTopTen(map<int, Movie> moviesMap)
{
    vector<pair<int, Movie>> topTenVec;
    for(auto s: moviesMap)
    {
        topTenVec.push_back(s);
    }

    sort(topTenVec.begin(), topTenVec.end(),
     [&](pair<int, Movie> m1, pair<int, Movie> m2)
     {
         if(m1.second.calcAvgRating() > m2.second.calcAvgRating())
            return true;
         else
            return false;
     });

     cout << ">> Top-10 Movies <<" << endl << endl;
     cout << "Rank    " << "ID      " << "Reviews " << "Avg     " << "Name    " << endl;

     for(int i = 0; i < 10; i++)
     {
         string dot = ".";
         cout << left << setw(8) << to_string(1+i)+dot
         << setw(8) << topTenVec[i].second.getMovieID()
         << setw(8) << topTenVec[i].second.getNumReviews()
         << setw(8) << topTenVec[i].second.calcAvgRating()
         << "'" << topTenVec[i].second.getMovieName() << "'" << endl;
     }
     cout << endl;
}


int main()
{
  //
  // input the filenames to process:
  //
  string moviesFN, reviewsFN;

  cin >> moviesFN;
  cin >> reviewsFN;

  cout << "** Netflix Movie Review Analysis **" << endl << endl;
  cout << moviesFN << endl;
  cout << reviewsFN << endl;
  cout << endl;

  auto moviesMap = inputMovies(moviesFN);
  auto reviewsMap = inputReviews(reviewsFN, moviesMap);

  printTopTen(moviesMap);


  //Interactive loop to search by movieID or reviewID
  cout << ">> Movie and Review Information <<" << endl << endl;
  long long int input = 1;
  while(input != 0)
  {
      cout << "Please enter a movie ID (< 100,000), a review ID (>= 100,000), 0 to stop> ";
      cin >> input;
      if(input < 100000 && input > 0)
      {
        auto movieIter = moviesMap.find(input);
        if(movieIter != moviesMap.end())
        {
          movieIter->second.printInformation();
        }
        else
        {
          cout << endl << "movie not found..." << endl << endl;
        }
      }
      if(input >= 100000)
      {
        auto reviewIter = reviewsMap.find(input);
        if(reviewIter != reviewsMap.end())
        {
            reviewIter->second.printInformation(moviesMap);
        }
        else
        {
            cout << endl << "review not found..." << endl << endl;
        }
      }
      if(input < 0)
      {
          cout << endl << "**invalid id..." << endl << endl;
      }
  }

  cout << "** DONE! **" << endl << endl;

  return 0;
}
