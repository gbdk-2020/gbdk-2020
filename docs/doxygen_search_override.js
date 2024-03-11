

// This file gets appended to the doxygen search.js to override and improve the search functions


// //////////////////

// Replacing a function in an existing object (function)

// var oldClass = mynamespace.myclass; // Copy original before overwriting
// mynamespace.myclass = function () {
//     // Apply the original constructor on this object
//     oldClass.apply(this, arguments);
//     // Now overwrite the target function after construction
//     this.myfunction = function () { alert("Overwritten"); };
// };
// mynamespace.prototype = oldClass.prototype; // Same prototype

// //////////////////


var oldSearchBox = SearchBox; // Copy original before overwriting
SearchBox = function () {
  // Apply the original constructor on this object
  oldSearchBox.apply(this, arguments);

  this.Search = function()
  {
    this.keyTimeout = 0;

    // strip leading whitespace
    var searchValue = this.DOMSearchField().value.replace(/^ +/, "");

    // var code = searchValue.toLowerCase().charCodeAt(0);
    // var idxChar = searchValue.substr(0, 1).toLowerCase();
    // if ( 0xD800 <= code && code <= 0xDBFF && searchValue > 1) // surrogate pair
    // {
    //   idxChar = searchValue.substr(0, 2);
    // }

    // var resultsPage;
    // var resultsPageWithSearch;
    // var hasResultsPage;

    // var idx = indexSectionsWithContent[this.searchIndex].indexOf(idxChar);
    // if (idx!=-1)
    // {
    //    var hexCode=idx.toString(16);
    //    resultsPage = this.resultsPath + '/' + indexSectionNames[this.searchIndex] + '_' + hexCode + this.extension;
    //    resultsPageWithSearch = resultsPage+'?'+escape(searchValue);
    //    hasResultsPage = true;
    // }
    // else // nothing available for this search term
    // {
    //    resultsPage = this.resultsPath + '/nomatches' + this.extension;
    //    resultsPageWithSearch = resultsPage;
    //    hasResultsPage = false;
    // }
    // CHANGED START
    var resultsPage;
    var resultsPageWithSearch;
    var hasResultsPage;

    resultsPage = this.resultsPath + '/' + 'combined.html';
    resultsPageWithSearch = resultsPage+'?'+escape(searchValue);
    hasResultsPage = true;
    // CHANGED END

    window.frames.MSearchResults.location = resultsPageWithSearch;
    var domPopupSearchResultsWindow = this.DOMPopupSearchResultsWindow();

    if (domPopupSearchResultsWindow.style.display!='block')
    {
       var domSearchBox = this.DOMSearchBox();
       this.DOMSearchClose().style.display = 'inline-block';
       if (this.insideFrame)
       {
         var domPopupSearchResults = this.DOMPopupSearchResults();
         domPopupSearchResultsWindow.style.position = 'relative';
         domPopupSearchResultsWindow.style.display  = 'block';
         var width = document.body.clientWidth - 8; // the -8 is for IE :-(
         domPopupSearchResultsWindow.style.width    = width + 'px';
         domPopupSearchResults.style.width          = width + 'px';
       }
       else
       {
         var domPopupSearchResults = this.DOMPopupSearchResults();
         var left = getXPos(domSearchBox) + 150; // domSearchBox.offsetWidth;
         var top  = getYPos(domSearchBox) + 20;  // domSearchBox.offsetHeight + 1;
         domPopupSearchResultsWindow.style.display = 'block';
         left -= domPopupSearchResults.offsetWidth;
         domPopupSearchResultsWindow.style.top     = top  + 'px';
         domPopupSearchResultsWindow.style.left    = left + 'px';
       }
    }

    this.lastSearchValue = searchValue;
    this.lastResultsPage = resultsPage;
  }
};
SearchBox.prototype = oldSearchBox.prototype; // Same prototype



var oldSearchResults = SearchResults; // Copy original before overwriting
SearchResults = function () {
  // Apply the original constructor on this object
  oldSearchResults.apply(this, arguments);


    this.Search = function(search)
    {
      if (!search) // get search word from URL
      {
        search = window.location.search;
        search = search.substring(1);  // Remove the leading '?'
        search = unescape(search);
      }

      search = search.replace(/^ +/, ""); // strip leading spaces
      search = search.replace(/ +$/, ""); // strip trailing spaces
      search = search.toLowerCase();
      // search = convertToId(search);
      // Commented out above (CHANGED START & END)

      var resultRows = document.getElementsByTagName("div");
      var matches = 0;

      var i = 0;
      while (i < resultRows.length)
      {
        var row = resultRows.item(i);
        if (row.className == "SRResult")
        {
          var rowMatchName = row.id.toLowerCase();
          // rowMatchName = rowMatchName.replace(/^sr\d*_/, ''); // strip 'sr123_'

          // if (search.length<=rowMatchName.length &&
          //    rowMatchName.substr(0, search.length)==search)
          // {
          // CHANGED START
          rowMatchName = rowMatchName.replace(/^[sr\d_]*5f_/, ''); // strip 'sr123_5f'
          rowMatchName = rowMatchName.replace(/5f/g, ''); // strip '5f' chars
          // rowMatchName = rowMatchName.replace(/_/, ' '); // underscores to spaces

          if (rowMatchName.includes(search))
          {
            // CHANGED END
            row.style.display = 'block';
            matches++;
          }
          else
          {
            row.style.display = 'none';
          }
        }
        i++;
      }
      document.getElementById("Searching").style.display='none';
      if (matches == 0) // no results
      {
        document.getElementById("NoMatches").style.display='block';
      }
      else // at least one result
      {
        document.getElementById("NoMatches").style.display='none';
      }
      this.lastMatchCount = matches;
      return true;
    }
};
SearchResults.prototype = oldSearchResults.prototype; // Same prototype

