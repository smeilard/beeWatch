int temps = 1000;

// fonction d'initialisation de la carte
void setup()
{
    // initialisation de la broche 2 comme étant une sortie
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
}


void eteintToutSauf( int n) {
  for ( int i=4; i<8; i++ ) {
    digitalWrite( i, HIGH);
  }
  digitalWrite( n, LOW);
}



// fonction principale, elle se répète (s’exécute) à l'infini
void loop()
{
  while ( true ) {
    // contenu de votre programme
    eteintToutSauf(4);
    delay(temps);
    eteintToutSauf(5);
    delay(temps);
    eteintToutSauf(6);
    delay(temps);
    eteintToutSauf(7);
    delay(temps);
    }
}
