class Station{
  public:
  Station(int loc, String drink){
    _loc = loc;
    _drink = drink;
  }
    
  void begin(){
    //pinMode(_locPin, INPUT);
  }
  
  int getLoc(){
    return _loc;
  }
  String getDrink(){
    return _drink;
  }

  private:
  int _loc;
  String _drink;
};
