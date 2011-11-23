
function getPage(url){
      // reads the last 'slash' / and takes text after it as a pagename
      return url.substring(url.lastIndexOf('/') + 1);
}

function highlightlink(curr_page, navlist){
      // this function will crosscheck between the menu links and
      // the current page and highlight the link in the menu
      //var link_array = new Array();

      // gets an reference of a object with the help of an id,
      // which will help in reading all the links under it.
      var mainnav = document.getElementById(navlist);
      if (mainnav == null)
      {
        return;
      }

      // reading all the the 'a' link tags
      // falling under the object identified by the passed id.
      // we get result as a collection of all 'a' object in an array
      var main_nav_links = mainnav.getElementsByTagName('a');

      // looping through collection of 'a'
      for (i = 0; i < main_nav_links.length; i++) {
        // reading a href value from the current 'a' tag in the loop             
        currHREF = main_nav_links[i].href;             
        currHREFPage = getPage(currHREF); 
                    
        // checks the obtained clean pagename with the curr_page            
        if (curr_page == currHREFPage) {                 
          // if found apply the class name,                 
          // which you want to apply to the specific menu item                      
          main_nav_links[i].className = "navcolor"; 
          return;             
        }
      }    
}

function navlink() {
    var url = window.location.pathname;
    // assigns a filename of the url to the curr_page variable
    var curr_page = getPage(url); 

    highlightlink(curr_page, "navtable");
}

   
function highlightText(id) {

	var textObject = document.getElementById(id);

	textObject.className = "highlight";

}

function changecolor(id)
{
	if(document.getElementById)
		document.getElementById(id).color = "red";
	else if(document.all)
		document.all[id].color = "red";

	return false;
}
