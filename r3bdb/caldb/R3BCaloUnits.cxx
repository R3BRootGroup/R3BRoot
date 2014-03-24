#include "R3BCaloUnits.h"


double Units::ToKelvin(double temperature) 
{ 
    return temperature; 
}

double Units::ToCelcius(double temperature)
{
    return temperature - standard_temperature;
}

double Units::ToFahrenheit(double temperature)
{
    return Units::ToCelcius(temperature)*9.0/5.0 + 32.0;
}

double Units::FromCelcius(double degrees_celcius)
{
    return degrees_celcius + standard_temperature;
}

double Units::FromKelvin(double degrees_kelvin)
{
    return degrees_kelvin;
}

double Units::FromFahrenheit(double degrees_fahrenheit)
{
    return FromCelcius(5.0/9.0*(degrees_fahrenheit-32));
}
