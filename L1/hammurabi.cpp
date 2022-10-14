#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <conio.h>

using namespace std;

const int maxAcreCost = 26;
const int minAcreCost = 17;
const double maxRatDamage = 0.07;
const int wheatPerHuman = 20;
const int maxAcresPerHuman = 10;
const double plagueProbability = 0.15;

const int wheatSowPerAcre = 5; //������� ������� ������� �� 1 ���
const int minWheatGetPerAcre = 1;
const int maxWheatGetPerAcre = 6;
const int roundsCount = 10;

const double pAcreCoef = 0.3;
const double pHungerCoef = 0.7;

int startRound;
int population;
int wheatCount;
int acresCount;

double hungerPercent;
double acresPerHumanInc;
double satietyRep;

double popularity;
double popularityByAcres;
double popularityBySatiety;

int hungerDeads;
int plagueDeads;
int wheatReceived;
int wheatLoss;
int populationInc;
int acreCost;
int wheatGetPerAcre;
int workAcres;
bool isPlague;
double ratDamage;

int getPopulationInc(int hungerDeads, int wheatGetPerAcre, int wheatCount)
{
    int c = hungerDeads / 2 + (5 - wheatGetPerAcre) * wheatCount / 600 + 1;
    c = c > 50 ? 50 : c < 0 ? 0 : c;
    return c;
}

double getPercent(double d)
{
    return (int)(d * 1000.0) / 10.0;
}

string getRepPhrase(double p)
{
    if(p < 0.3)
        return "���� ���������� ���!";
    else if(p < 0.5)
        return "���� ��������� ���!";
    else if(p < 0.70)
        return "���� ������ ���.";
    else if(p < 0.80)
        return "���� ������� �� ��� ��������.";
    else if(p < 0.95)
        return "���� ����� ���!";
    else
        return "���� ����������� ����!";
}

void setGameData(bool fromFile = false)
{
    if(fromFile)
    {
        ifstream fin("save.txt");
        fin >> startRound;
        fin >> population;
        fin >> wheatCount;
        fin >> acresCount;

        fin >> hungerPercent;
        fin >> acresPerHumanInc;
        fin >> satietyRep;

        fin >> popularity;
        fin >> popularityByAcres;
        fin >> popularityBySatiety;

        fin >> hungerDeads;
        fin >> plagueDeads;
        fin >> wheatReceived;
        fin >> wheatLoss;
        fin >> populationInc;
        fin >> acreCost;
        fin >> wheatGetPerAcre;
        fin >> workAcres;
        fin >> isPlague;
        fin >> ratDamage;
        fin.close();
    }
    else
    {
        startRound = 1;
        population = 100;
        wheatCount = 2800;
        acresCount = 1000;

        hungerPercent = 0;
        acresPerHumanInc = (double)acresCount / (double)population / (double)maxAcresPerHuman;
        satietyRep = 0;

        popularity = 1;
        popularityByAcres = pAcreCoef;
        popularityBySatiety = pHungerCoef;

        hungerDeads = 0;
        plagueDeads = 0;
        wheatReceived = 0;
        wheatLoss = 0;
        populationInc = 0;
        acreCost = rand() % (maxAcreCost - minAcreCost + 1) + minAcreCost;
        wheatGetPerAcre = 0;
        workAcres = 0;
        isPlague = false;
        ratDamage = 0;
    }
}

void saveGameData()
{
    ofstream fout("save.txt");
    fout << startRound << endl;
    fout << population << endl;
    fout << wheatCount << endl;
    fout << acresCount << endl;

    fout << hungerPercent << endl;
    fout << acresPerHumanInc << endl;
    fout << satietyRep << endl;

    fout << popularity << endl;
    fout << popularityByAcres << endl;
    fout << popularityBySatiety << endl;

    fout << hungerDeads << endl;
    fout << plagueDeads << endl;
    fout << wheatReceived << endl;
    fout << wheatLoss << endl;
    fout << populationInc << endl;
    fout << acreCost << endl;
    fout << wheatGetPerAcre << endl;
    fout << workAcres << endl;
    fout << isPlague << endl;
    fout << ratDamage << endl;
    fout.close();
}

bool checkSaveFileExist()
{
    fstream fs;
    fs.open("save.txt");
    bool isFail = !fs.fail();
    fs.close();
    return isFail;

}

int main()
{
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));

    int c = 72;
    bool saveExist = checkSaveFileExist();

    while(1){
        system("cls");
        cout << "** Hammurabi game **" << endl;
        cout << "   " << (c == 72 ? ">" : " ") << " ����� ����" << endl;
        if(saveExist)
            cout << "   " << (c == 80 ? ">" : " ") << " ���������� ����" << endl;
        cout << endl << "Up arrow/Down arrow - ������� �����, Enter - �������" << endl;
        int in = getch();
        if(in == 13)
            break;
        if(in == 72 || in == 80 && saveExist)
            c = in;
    }

    setGameData(c == 80);

    for(int i = startRound; i <= roundsCount; i++)
    {
        saveGameData();
        system("cls");
        cout << endl;
        cout << "���: " << i << endl;
        cout << "���� ������������: " << getPercent(popularity) << "%" << endl;
        cout << "   " << (popularityBySatiety - pHungerCoef >= 0 ? "+" : "") << getPercent(popularityBySatiety - pHungerCoef) << "% �� ������� ���������" <<  endl;
        cout << "   " << (popularityByAcres - pAcreCoef >= 0 ? "+" : "") << getPercent(popularityByAcres - pAcreCoef) << "% �� ���������� ����� �� ��������" <<  endl;

        cout << getRepPhrase(popularity) << endl;

        if(isPlague)
            cout << "������ �������! � ������� ���� �������� ����!" << endl;
        cout << endl;
        cout << " ���������: " << population << endl;
        if(populationInc > 0)
            cout << "   +" << populationInc << " ������ � �����" << endl;
        if(hungerDeads > 0)
            cout << "   -" << hungerDeads << " ������ �� ������" << endl;
        if(plagueDeads > 0)
            cout << "   -" << plagueDeads << " ������ �� ����" << endl;

        cout << endl;
        cout << " ������ �������: " << wheatCount << endl;
        if(wheatReceived > 0)
            cout << "   +" << wheatReceived << " �������� ( +" << wheatGetPerAcre << " �� ���)" << endl;
        if(wheatLoss > 0)
            cout << "   -" << wheatLoss << " ����� �����" << endl;
        cout << endl;
        cout << " ���-�� �����: " << acresCount << endl;
        cout << "   " << workAcres << " - ������������" << endl;

        cout << " ��������� ����: " << acreCost << endl;
        cout << endl;
        cout << " (?) ���������� ������� ������� ��� ������� ������: " << min(acresCount, population * maxAcresPerHuman) * wheatSowPerAcre << endl;
        cout << " (?) ���������� ������� ��� ����������: "<< wheatPerHuman * population << " �� " << population << " �������(�)" << endl;
        cout << endl;
        cout << "Enter - ������ �����, Esc - ����� �� ����" << endl;
        cout << endl;

        while(1){
            int in = getch();
            if(in == 13){
                break;
            }
            if(in == 27){
                saveGameData();
                return 0;
            }
        }


        wheatGetPerAcre = rand() % (maxWheatGetPerAcre - minWheatGetPerAcre + 1) + minWheatGetPerAcre;
        isPlague = ((double)rand() / (double)RAND_MAX) <= plagueProbability;
        ratDamage = ((double)rand() / (double)RAND_MAX) * maxRatDamage;

        int acresChanging;
        int wheatForEat;
        int wheatForSow;
        int wheatChange;

        while(1)
        {
            wheatChange = 0;
            cout << "(������� ����� �������: " << wheatCount - wheatChange << ")" << " ������� ����� ������/�������:"; cin >> acresChanging;
            wheatChange += acresChanging * acreCost;
            cout << "(������� ����� �������: " << wheatCount - wheatChange << ")" << " ������� ������� �������:"; cin >> wheatForEat;
            wheatChange += wheatForEat;
            cout << "(������� ����� �������: " << wheatCount - wheatChange << ")" << " ������� ������� �������:"; cin >> wheatForSow;
            wheatChange += wheatForSow;
            if(wheatChange > wheatCount || wheatForEat < 0 || wheatForSow < 0 || acresCount + acresChanging < 0){
                cout << "�, ����������, ������ ���! �� �� �������� ��������� ���� �����!" << endl;
                continue;
            }
        }
        wheatCount -= wheatChange;

        acresCount += acresChanging;

        workAcres = min(min(wheatForSow / wheatSowPerAcre, acresCount), population * maxAcresPerHuman);

        wheatReceived = workAcres * wheatGetPerAcre;

        wheatCount += wheatReceived;

        wheatLoss = (int)((double)wheatCount * ratDamage);

        wheatCount = max(wheatCount - wheatLoss, 0);

        satietyRep += ((double)wheatForEat / (double)wheatPerHuman) / (double)population;

        hungerDeads = max(population - (wheatForEat / wheatPerHuman), 0);

        acresPerHumanInc = min((double)acresCount / (double)population / (double)maxAcresPerHuman, 1.2);

        hungerPercent += (double)hungerDeads / (double)population;

        population -= hungerDeads;

        plagueDeads = isPlague ? population / 2 : 0;

        population -= plagueDeads;


        populationInc = getPopulationInc(hungerDeads, wheatGetPerAcre, wheatCount);

        population += populationInc;

        if(population == 0)
            break;


        popularityByAcres = acresPerHumanInc * pAcreCoef;
        popularityBySatiety = satietyRep / i * pHungerCoef;

        popularity = popularityBySatiety + popularityByAcres;
        popularity = popularity < 0 ? 0 : popularity > 1 ? 1 : popularity;

        acreCost = rand() % (maxAcreCost - minAcreCost + 1) + minAcreCost;
    }

    cout << endl;
    cout << "���� ��������!" << endl;
    cout << "������������ ������ ��������� ���������: " << getPercent(popularity) << "%" << endl;
    cout << getRepPhrase(popularity) << endl;

    cout << "�� ���� ���������:" << endl;
    cout << "   - " << (int)(hungerPercent / (double)roundsCount * 1000.0) / 10.0 << "% ��������� ������ �� ������" << endl;
    cout << "   - ������ ������ ������ ����������� �����: " << acresPerHumanInc * maxAcresPerHuman << " ����� �����" << endl;

    return 0;
}
