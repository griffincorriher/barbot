
// Creates dropdown menu for the user to select the drink they want to order
var $select = $("#alcoholic");
$.getJSON('drinks.json', function(drinks) {
    $select.html('');
    const alcholicOptions = ["Vodka Drinks", "Tequila Drinks", "Rum Drinks"];
    //$select.append(<option id="" disabled selected>Select your drink</option>);
    for(var i = 0; i < drinks['Alcoholic'].length; i++) {
        $select.append('<optgroup label="' + alcholicOptions[i] + '">' + alcholicOptions[i] + '</optgroup>');
        for(var j = 0; j < drinks['Alcoholic'][i].length; j++) {
        $select.append('<option id="' + drinks['Alcoholic'][i][j].Name + '">' + drinks['Alcoholic'][i][j].Name + '</option>');
        }
    }
});

var $select2 = $("#nonalcoholic");
$.getJSON('drinks.json', function(drinks) {
    $select2.html('');
    const nonAlcholicOptions = ["Soda", "Juice", "Coffee"];
    for(var i = 0; i < drinks['Non-Alcoholic'].length; i++) {
      $select2.append('<optgroup label="' + nonAlcholicOptions[i] + '">' + nonAlcholicOptions[i] + '</optgroup>');
      for(var j = 0; j < drinks['Non-Alcoholic'][i].length; j++) {
        $select2.append('<option id="' + drinks['Non-Alcoholic'][i][j].Name + '">' + drinks['Non-Alcoholic'][i][j].Name + '</option>');
        }
    }
    
});

// Updates "currently making" drink
setInterval(function ( ) 
  {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200)
       {
        document.getElementById("current-drink").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/currentdrink", true);
    xhttp.send();
  }, 1000 ) ;


  // Updates number of drinks in queue
  setInterval(function ( ) 
  {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200)
       {
        document.getElementById("drink-queue").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/drinkqueue", true);
    xhttp.send();
  }, 1000 ) ;  

// Creates popup when drink is requested
var drinkRequest;
$('#frm1').submit(function() {
  var c = confirm("Make a " + $('#alcoholic').val() + "?");
//  drinkRequest = $('#alcoholic').val();
//  sendDrink(drinkRequest);

});

$('#frm2').submit(function() {
  var c = confirm("Make a "+ $('#nonalcoholic').val() + "?");
//  drinkRequest = $('#nonalcoholic').val();
//  sendDrink(drinkRequest);
});

// Send drink variable to Wemos D1
//function sendDrink(request){
//  console.log(drinkRequest)
//  var xhttp = new XMLHttpRequest();
//  xhttp.open("GET", "/get", true);
//  xhttp.send();   
//}