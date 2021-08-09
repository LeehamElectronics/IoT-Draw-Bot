// TODO: Replace with your project's config object. You can find this
// by navigating to your project's console overview page
// (https://console.firebase.google.com/project/your-project-id/overview)
// and clicking "Add Firebase to your web app"
var config = {
 apiKey: "REDACTED",
    authDomain: "REDACTED",
    databaseURL: "REDACTED",
    projectId: "REDACTED",
    storageBucket: "REDACTED",
    messagingSenderId: "REDACTED",
    appId: "REDACTED"

};

// Initialize your Firebase app
firebase.initializeApp(config);

// Reference to the recommendations object in your Firebase database
var movement = firebase.database().ref("movement");
var pencilDown = firebase.database().ref("pencilDownAngle");
var pencilUp = firebase.database().ref("pencilUpAngle");
var REFBVal = firebase.database().ref("RE");
var FEFBVal = firebase.database().ref("FE");
var letterEntry = firebase.database().ref("letterString");
FEFBVal.on('value', snap => console.log(snap.val()));
/* Save a new recommendation to the database, using the input in the form
var submitRecommendation = function () {

  // Get input values from each of the form elements
  var title = $("#talkTitle").val();
  var presenter = $("#talkPresenter").val();
  var link = $("#talkLink").val();

  // Push a new recommendation to the database using those values
  recommendations.push({
    "title": title,
    "presenter": presenter,
    "link": link
  });
};
*/
// When the window is fully loaded, call this function.
// Note: because we are attaching an event listener to a particular HTML element
// in this function, we can't do that until the HTML element in question has
// been loaded. Otherwise, we're attaching our listener to nothing, and no code
// will run when the submit button is clicked.
$(window).load(function () {

  // Find the HTML element with the id recommendationForm, and when the submit
  // event is triggered on that element, call submitRecommendation.
  $("#recommendationForm").submit(submitRecommendation);

});
