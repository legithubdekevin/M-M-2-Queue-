#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double lambda, mu, timeMAX;
// Our random generator

#define norm 2.328306549295728e-10
#define m1 4294967087.0
#define m2 4294944443.0
#define a2 1403580.0
#define a3 810728.0
#define b1 527612.0
#define b3 1370589.0

static double
    s10 = 12345,
    s11 = 12345, s12 = 123,
    s20 = 12345, s21 = 12345, s22 = 123;

double MRG32k3a(void)
{
    long k;
    double p1, p2;

    /* Suite x */
    p1 = a2 * s11 - a3 * s10;
    k = p1 / m1;
    p1 -= k * m1;
    if (p1 < 0.0)
        p1 += m1;
    s10 = s11;
    s11 = s12;
    s12 = p1;

    /* Suite y */
    p2 = b1 * s22 - b3 * s20;
    k = p2 / m2;
    p2 -= k * m2;
    if (p2 < 0.0)
        p2 += m2;
    s20 = s21;
    s21 = s22;
    s22 = p2;

    /* Combinaison */
    if (p1 <= p2)
        return ((p1 - p2 + m1) * norm);
    else
        return ((p1 - p2) * norm);
}

// End of generator

double generateRandExp(double lambda)
{
    double u;
    u = MRG32k3a();
    return -log(u) / lambda;
}

double isInInterval()
{

    double x, value;

    x = MRG32k3a();

    // Counts 1 if the generated point is in the interval
    if (x >= 1.0 && x <= 2.0)
        value = 1.0;
    else
        value = 0.0;

    return (value);
}

int main(int nbOfPars, char **pars)
{
    // Number in the queue
    long N = 0;
    // Total of people that come
    double sumN = 0;
    // Average number of customer
    double nAverage = 0;

    double time = 0;

    // 1 if the server is available
    int server1Able = 1;
    int server2Able = 1;

    // Time to know what is the next event. Initiate at timeMAX * 2 means they are free
    double time_next_departure1 = timeMAX * 2.;
    double time_next_departure2 = timeMAX * 2.;

    double time_next_arrival = 0;

    // Var to know what is the next Event (arrival or departure on server 1 or 2 )
    double nextEvent;

    // input parameters
    // command example to test : ./tp3KMPM 35 30 1000
    lambda = atol(pars[1]);
    mu = atol(pars[2]);
    timeMAX = atol(pars[3]);

    // Variable to verify our values
    double rho = lambda / (2.0 * mu);

    // Initialisation
    time_next_arrival = generateRandExp(lambda);

    sumN = N * (time_next_arrival - time) + sumN;

    time = time_next_arrival;

    while (time < timeMAX)
    {
        // check what is the next event in the time line
        nextEvent = fmin(fmin(time_next_departure1, time_next_departure2), time_next_arrival);
        // next event is an arrival
        if (nextEvent == time_next_arrival)
        {
            // For the average calculation
            sumN = N * (nextEvent - time) + sumN;

            // set the time and add one customer in the queue
            time = nextEvent;
            N++;

            // if there is less or equal 2 customers in the queue, they are automaticly in a server
            if (N <= 2)
            {

                // check if the server1 is available else it mean the server 2 is able to know in which server the customer will be
                // we generate the time of the next departure
                if (server1Able)
                {
                    server1Able = 0;
                    time_next_departure1 = generateRandExp(mu) + time;
                }
                else
                {
                    server2Able = 0;
                    time_next_departure2 = generateRandExp(mu) + time;
                }
            }

            // arrival has been done so we can prepare the next arrival on the timeline
            time_next_arrival = generateRandExp(lambda) + time;
        }
        // the commentary applied here is the same for the time_next_departure2
        else if (nextEvent == time_next_departure1)
        {

            // at least one customer is in the queue
        
            sumN += N * (nextEvent - time);

            time = nextEvent;
            N--;
            if(N<=1)
            {
                server1Able = 1;
                time_next_departure1 = timeMAX * 2.;

            } else{

                server1Able = 0;
                time_next_departure1 = generateRandExp(mu) + time;

            }
        }
        else if (nextEvent == time_next_departure2){
        
            sumN += N * (nextEvent - time);

            time = nextEvent;
            N--;
            if(N<=1)
            {
                server2Able = 1;
                time_next_departure2 = timeMAX * 2.;

            } else{

                server2Able = 0;
                time_next_departure2 = generateRandExp(mu) + time;

            }
        }
    }
    nAverage = sumN;

    printf("Average number of customers: %f \n", nAverage / time);

    printf("Expected value %f \n", (2.0 * rho) / (1.0 - (rho * rho)));
}
