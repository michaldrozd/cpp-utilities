// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include "curl/curl.h"

using namespace std;

// Global variables for threads synchronization and data sharing.
mutex mtx; // mutex for critical section
condition_variable cv; // condition variable for threads synchronization
queue<string> q; // queue of URLs to be processed
vector<string> visited; // vector of visited URLs


// Function to write data from web page to file.
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);

	return written;
}

// Function to extract URLs from webpage.
void extractURLs(const string& webpage)
{
	int pos = 0;
	while (true)
	{
		int found = webpage.find("a href", pos); // Find position of "a href" in webpage. If not found then break the loop.
		if (found == string::npos)
		{ break; }
		int start = webpage.find('"', found); // Find position of '"' after "a href". If not found then break the loop.
		if (start == string::npos)
		{ break; }
		int end = webpage.find('"', start + 1); // Find position of '"' after '"'. If not found then break the loop.
		if (end == string::npos)
		{ break; }
		// Extract URL from webpage between '"' and '"'. If URL is not absolute then prepend it with "https://en.wikipedia.org".
		// If URL is already visited then continue with next iteration of loop. Otherwise, add it to queue and vector of visited URLs.
		string url = webpage.substr(start + 1, end - start - 1);
		if (url[0] == '/')
		{
			url = "https://en.wikipedia.org" + url;
		}

		if (url[0] == '#')
		{ continue; }

		if (url.find("https://en.wikipedia.org") != std::string::npos)
		{
			continue;
		}

		bool alreadyVisited = false;

		for (const auto & i : visited)
		{
			if (i == url)
			{
				alreadyVisited = true;
				break;
			}
		}

		if (!alreadyVisited)
		{
			q.push(url);
			visited.push_back(url);
		}
		// Set new position for next iteration of loop to character after last character of extracted URL in order
		// to avoid extracting same URL again and again in next iterations of loop when it occurs multiple times
		// on same web page as it does on Wikipedia web pages e.g.: https://en.wikipedia.org/wiki/Main_Page .
		pos = end + 1;
	}
}

// Function executed by threads to process URLs from queue one by one until queue is empty and all threads are waiting on condition variable
// which means that there are no more URLs to be processed and all threads can terminate their execution safely without any data loss or race conditions
// e.g.: when one thread is processing last URL from queue and other thread is waiting on condition variable
// and wakes up just before first thread finishes its execution and pops last URL from queue which results in empty queue
// and second thread will wait forever on empty queue because first thread has already terminated its execution
// so there will be no more URLs added to queue by any thread so second thread will wait forever on empty queue until program is terminated by user or operating system e.g.: when computer is turned off.
[[noreturn]] void processURLs()
{
	while (true)
	{
		unique_lock<mutex> lck(mtx); // Lock mutex.

		// Wait on condition variable until queue is not empty.
		cv.wait(lck, []
		{ return !q.empty(); });
		// Get URL from queue.
		string url = q.front();
		// Pop URL from queue.
		q.pop();
		// Unlock mutex.
		lck.unlock();
		// Pointer to CURL object for libcurl library to download web page from URL and extract URLs from it.
		CURL* curl;
		// Pointer to file stream to write data from web page to file.
		FILE* fp;
		// Result of libcurl library function calls e.g.: curl_easy_setopt(), curl_easy_perform() e.t.c.
		CURLcode res;
		// String to store data from web page downloaded by libcurl library function curl_easy_perform().
		string webpage;
		// Initialize CURL object for libcurl library to download web page from URL and extract URLs from it.
		curl = curl_easy_init();
		if (curl)
		{

			string filename;

			for (char i : url)
			{
				if (i == '/')
				{ filename += "_"; }
				else
				{ filename += i; }
			}

			fp = fopen(filename.c_str(), "wb");

			const char* c_url = url.c_str();
			curl_easy_setopt(curl, CURLOPT_URL, c_url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(fp);
		}
	}
}

// Main function of program to create 12 threads and start their execution by calling processURLs() function in each thread
// and wait for all threads to finish their execution before terminating main thread's execution and program's execution
// as well because main thread is the only thread left after all other threads have finished their execution
// so when main thread finishes its execution program will be terminated by operating system
// e.g.: when computer is turned off or user terminates program's execution by pressing Ctrl+C in terminal window
// in which program is running or by closing terminal window in which program is running or by closing IDE in which program is running e.t.c..
int main()
{
	vector<thread> threads;

	for (int i = 0; i < 12; i++)
	{ threads.emplace_back(processURLs); }

	q.emplace("https://en.wikipedia.org/wiki/Main_Page");
	visited.emplace_back("https://en.wikipedia.org/wiki/Main_Page");
	extractURLs("https://en.wikipedia.org/wiki/Main_Page");
	cv.notify_all();

	for (auto& th: threads)
	{ th.join(); }

	return 0;
}