//
//  DBInterfaceCSV.hpp
//  DER
//
//  Created by Cees Carels on 31/03/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef DBInterfaceCSV_hpp
#define DBInterfaceCSV_hpp

#include <sstream>
#include <stdio.h>
#include <string>

#include "DBInterface.hpp"

/**
 * Implementation of DBInterface base class for CSV files.
 *
 * Example usage of class:
 * DBInterface* myCSV = new DBInterfaceCSV();
 * myCSV->setConfig("/Users/ceescarels/Desktop/test.txt");
 * if(!myCSV->isOK()) {
 *      std::cout << "Could not open CSV file" << std::endl;
 * }
 *
 * std::cout << myCSV->getConfig("title3", 0) << std::endl;
 *
 * The CSV file layout assumes the row index for data starts at row 1
 * i.e. in the lookup one ignores the existence of titles and data rows start at
 * row 0 (as shown in the example).
 *
 * The lookup only looks for the correct column using the first argument, such
 * that the second parameter gives the row number.
 *
 * A lookup by row index number would require an extra row index to be
 * implemented.
 *
 * The factory methods that are provided for these classes could also be used.
 * The example usage is given for the DBInterfaceConfig.
 *
 * When using the factory methods, be careful to de-allocate the memory for
 * these objects in the right place.
 *
 * Another improvement this class could benefit from is to read the file
 * file as it needs the data, rather than loading it all into memory.
 *
 * This could be accomplished by retaining a simply offset and repeating the
 * getline() methods shown in the setConfig method when the file is being read.
 */

template <class TYPE>
class DBInterfaceCSV : public DBInterface<TYPE>
{
public:
    DBInterfaceCSV();
    ~DBInterfaceCSV();

    void doConnect();
    bool doDisconnect();
    bool doCheckConnection();

    void RetrieveParameter(const std::string& name);

    void setConfig(const std::string& path);
    void setConfigSize(std::vector<std::string> titlevec, unsigned long size2);

    int CLISet(const std::string& SID, const std::string val);
    TYPE getConfig(const std::string& SID);
    TYPE getConfig(const std::string& SID, const unsigned int Entry);
    void setConfig(const std::string& SID, const unsigned int Entry, TYPE val);

    bool isOK();

    void Notification(std::string& par1,
        std::string& par2,
        std::string& par3);

    TYPE getALL();

protected:
    std::ifstream dat;
    unsigned long NRow; //Number of rows in the file
    unsigned long NCol; //Number of columns in the file
    std::vector<std::string> titles; //Used for title lookup
    std::vector<std::vector<TYPE>> data;
};

template <class TYPE>
DBInterfaceCSV<TYPE>::DBInterfaceCSV()
{
    /**
     * Constructor for DBInterfaceCSV.
     */
}

template <class TYPE>
DBInterfaceCSV<TYPE>::~DBInterfaceCSV()
{
    /**
     * Destructor for DBInterfaceCSV.
     */
}

template <class TYPE>
void DBInterfaceCSV<TYPE>::doConnect()
{
    /**
     * Method intentionally left blank.
     */
}

template <class TYPE>
bool DBInterfaceCSV<TYPE>::doDisconnect()
{
    return true;
}

template <class TYPE>
bool DBInterfaceCSV<TYPE>::doCheckConnection()
{
    return true;
}

template <class TYPE>
void DBInterfaceCSV<TYPE>::RetrieveParameter(const std::string& name)
{
    /**
     * Method intentionally left blank.
     */
}

template <>
void DBInterfaceCSV<std::string>::setConfig(const std::string& path)
{
    std::string edel = ","; //Delimeter for entries on a given line
    std::string ldel = ";"; //Delimeter for new line

    dat.open(path.c_str());

    if (!dat.is_open())
    {
        DBInterface<std::string>::confIsOK = false;
        std::cout << "Data file could not be opened: " << path << std::endl;
    }
    else
    {
        DBInterface<std::string>::confIsOK = true;
        NRow = 0;
        std::cout << "Data file opened: " << path << std::endl;
        while (dat.good())
        {
            std::string temp;
            getline(dat, temp); //Get row
            ++NRow;
        }

        dat.clear();
        dat.seekg(0, std::ios::beg);

        std::vector<std::string> row;
        row.resize(NRow);

        unsigned long long cnt = 0;
        while (dat.good())
        {
            getline(dat, row[cnt]); //Get row
            ++cnt;
        }

        unsigned long titllinestart = 0; //Line where titles start
        //Learn the titles for the columns by reading row 0
        //Associate each string to an index number
        std::string s = row[0];
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(edel)) != std::string::npos)
        {
            token = s.substr(0, pos);
            if (token == "")
            {
                std::cout << "Error: Empty token not allowed" << std::endl;
            }
            titles.push_back(token); //Contains the name as on statement.
            s.erase(0, pos + edel.length());
        }
        titles.push_back(s);

        data.resize(titles.size()); //Number of columns is the outer, slow index
        for (int i = 0; i < titles.size(); i++)
        {
            data[i].resize(row.size() - 1);
        }

        for (unsigned long i = titllinestart + 1; i < row.size(); i++)
        {
            //This code is needed to explicitly cast the string from the file to a char
            //This is needed in order to compare it to the delimiter
            //The comparison follows immediately
            std::stringstream ss;
            std::string end;
            char c = row[i].back();
            ;
            ss << c;
            ss >> end;

            s = row[i];
            std::string delimiter = ",";
            size_t pos = 0;
            std::string token;

            int j = 0; //Column index
            while ((pos = s.find(delimiter)) != std::string::npos)
            {
                token = s.substr(0, pos);

                if (token == "")
                {
                    std::cout << "Error: empty entry." << std::endl;
                }

                data[j][i - 1] = token;
                ++j;
                s.erase(0, pos + delimiter.length()); //s contains the rest of the string
            }
            data[j][i - 1] = s;
        }
        dat.close();
    }
}

template <>
void DBInterfaceCSV<double>::setConfig(const std::string& path)
{
    std::string edel = ","; //Delimeter for entries on a given line
    std::string ldel = ";"; //Delimeter for new line

    dat.open(path.c_str());

    if (!dat.is_open())
    {
        confIsOK = false;
        std::cout << "Data file could not be opened: " << path << std::endl;
    }
    else
    {
        confIsOK = true;
        NRow = 0;
        std::cout << "Data file opened: " << path << std::endl;
        while (dat.good())
        {
            std::string temp;
            getline(dat, temp); //Get row
            ++NRow;
        }

        dat.clear();
        dat.seekg(0, std::ios::beg);

        std::vector<std::string> row;
        row.resize(NRow);

        unsigned long long cnt = 0;
        while (dat.good())
        {
            getline(dat, row[cnt]); //Get row
            ++cnt;
        }

        unsigned long titllinestart = 0; //Line where titles start
        //Learn the titles for the columns by reading row 0
        //Associate each string to an index number
        std::string s = row[0];
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(edel)) != std::string::npos)
        {
            token = s.substr(0, pos);
            if (token == "")
            {
                std::cout << "Error: Empty token not allowed" << std::endl;
            }
            titles.push_back(token); //Contains the name as on statement.
            s.erase(0, pos + edel.length());
        }
        titles.push_back(s);

        data.resize(titles.size()); //Number of columns is the outer, slow index
        for (int i = 0; i < titles.size(); i++)
        {
            data[i].resize(row.size() - 1);
        }

        for (unsigned long i = titllinestart + 1; i < row.size(); i++)
        {
            //This code is needed to explicitly cast the string from the file to a char
            //This is needed in order to compare it to the delimiter
            //The comparison follows immediately
            std::stringstream ss;
            std::string end;
            char c = row[i].back();
            ;
            ss << c;
            ss >> end;

            s = row[i];
            std::string delimiter = ",";
            size_t pos = 0;
            std::string token;

            int j = 0; //Column index
            while ((pos = s.find(delimiter)) != std::string::npos)
            {
                token = s.substr(0, pos);

                if (token == "")
                {
                    std::cout << "Error: empty entry." << std::endl;
                }
                data[j][i - 1] = std::stod(token);
                ++j;
                s.erase(0, pos + delimiter.length()); //s contains the rest of the string
            }
            data[j][i - 1] = std::stod(s);
        }
        dat.close();
    }
}

template <>
void DBInterfaceCSV<double>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    titles.resize(titlevec.size());
    for (int i = 0; i < titles.size(); i++)
    {
        titles[i] = titlevec[i];
    }

    data.resize(titles.size()); //Number of columns is the outer, slow index
    for (int i = 0; i < titles.size(); i++)
    {
        data[i].resize(size2);
    }
}

template <>
void DBInterfaceCSV<std::string>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
}

template <class TYPE>
int DBInterfaceCSV<TYPE>::CLISet(const std::string& SID, const std::string val)
{
    /**
     * Modify a value in the CSV file.
     */
    return -1;
}

template <class TYPE>
TYPE DBInterfaceCSV<TYPE>::getConfig(const std::string& SID)
{
    return TYPE();
}

template <class TYPE>
TYPE DBInterfaceCSV<TYPE>::getConfig(const std::string& SID, const unsigned int Entry)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    for (int i = 0; i < titles.size(); i++)
    {
        if (SID == titles[i])
        {
            if (Entry < data[i].size())
                return data[i][Entry];
            else
                break;
        }
    }
    std::cout << "Error: Entry not found in DBInterfaceCSV." << std::endl;
    return TYPE();
}

template <>
void DBInterfaceCSV<double>::setConfig(const std::string& SID, const unsigned int Entry, double val)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    for (int i = 0; i < titles.size(); i++)
    {
        if (SID == titles[i])
        {
            if (Entry < data[i].size())
            {
                data[i][Entry] = val;
                break;
            }
            else
                break;
        }
    }
}

template <>
void DBInterfaceCSV<std::string>::setConfig(const std::string& SID, const unsigned int Entry, std::string val)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    for (int i = 0; i < titles.size(); i++)
    {
        if (SID == titles[i])
        {
            if (Entry < data[i].size())
            {
                data[i][Entry] = val;
                break;
            }
            else
                break;
        }
    }
}

template <class TYPE>
void DBInterfaceCSV<TYPE>::Notification(std::string& par1,
    std::string& par2,
    std::string& par3)
{
}

template <class TYPE>
TYPE DBInterfaceCSV<TYPE>::getALL()
{
    return TYPE();
}

#endif /* DBInterfaceCSV_hpp */
