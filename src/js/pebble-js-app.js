function number_format(number, decimals, dec_point, thousands_sep) {
  number = (number + '').replace(/[^0-9+\-Ee.]/g, '');
  var n = !isFinite(+number) ? 0 : +number,
    prec = !isFinite(+decimals) ? 0 : Math.abs(decimals),
    sep = (typeof thousands_sep === 'undefined') ? ',' : thousands_sep,
    dec = (typeof dec_point === 'undefined') ? '.' : dec_point,
    s = '',
    toFixedFix = function (n, prec) {
      var k = Math.pow(10, prec);
      return '' + Math.round(n * k) / k;
    };
  // Fix for IE parseFloat(0.55).toFixed(0) = 0;
  s = (prec ? toFixedFix(n, prec) : '' + Math.round(n)).split('.');
  if (s[0].length > 3) {
    s[0] = s[0].replace(/\B(?=(?:\d{3})+(?!\d))/g, sep);
  }
  if ((s[1] || '').length < prec) {
    s[1] = s[1] || '';
    s[1] += new Array(prec - s[1].length + 1).join('0');
  }
  return s.join(dec);
}

function findWhen(env) {
  var when = '';
  switch (env) {
    case 0:
      when = 'today'; break;
    case 1:
      when = 'week'; break;
    case 2:
      when = 'month'; break;
    case 3:
      when = 'year'; break;
    case 4:
      when = 'alltime'; break;
  }
  return when;
}

function displayHits(hits) {
  var count = '', hits = hits + '';
  switch (hits.length) {
    case 1:
    case 2:
    case 3:
      count += hits;
      break;
    case 4:
    case 5:
      count += number_format(hits);
      break;
    case 6:
      count = (hits/1000).toFixed(1) + 'K';
      break;
    case 7:
    case 8:
      count = (hits/1000000).toFixed(1) + 'M';
      break;
  }
  return count;
}

function sendRequest(url, post, cb) {
  var req = new XMLHttpRequest();
  req.open('POST', url, true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      console.log(req.responseText);
      var response = JSON.parse(req.responseText);
      cb(response, e);
    }
  };
  console.log(url);
  console.log(post);
  req.send(post);
}

function getInStorage(k, d) {
  var v = localStorage.getItem(k);
  if (!v) {
    v = d;
  }
  return v;
}

function setInStorage(k, v, d) {
  if (typeof v == typeof d) {
    localStorage.setItem(k, v);
  }
  return v;
}

function pebbleMessageSuccess(e) {
  console.log('Successfully delivered message with transactionId: ' + e.data.transactionId);
}

function pebbleMessageFailure(e) {
  console.log('Unable to deliver message with transactionId=' + e.data.transactionId, e.error);
}

/************************************************************************/

var url = 'http://hits-pebbleapps.rhcloud.com/';
var email = '';
var password = '';
var profile = '';
var website = '';
var env = 0;
var when = 'today';

/**
 * App Ready Listener
 *
 * The ready listener fires when the app is selected from the menu.
 *
 */
Pebble.addEventListener('ready', function(e) {
  console.log('JavaScript app ready and running!');
  email    = getInStorage('email', '');
  password = getInStorage('password', '');
  profile  = getInStorage('profile', '');
  website  = getInStorage('website', '');
  env      = getInStorage('env', 0);
  when     = getInStorage('when', 'today');
	
  var post = "email="+encodeURIComponent(email)+"&password="+encodeURIComponent(password)+"&profile="+encodeURIComponent(profile)+"&website="+encodeURIComponent(website);
	
  sendRequest(url + 'pebble.php?when=' + when, post, function(response) {
    Pebble.sendAppMessage({ '1': displayHits(response.count), '2': response.when, '3': website }, pebbleMessageSuccess, pebbleMessageFailure);
  });
});

/**
 * App Message Listener
 *
 * The app message listener sends a payload object to this script via
 * the an event variable 'e'. The payload consists of keys set up in
 * the C file for the app:
 *
 * 	KEY_ENV     = 0,
 *	KEY_COUNT   = 1,
 *	KEY_TITLE   = 2,
 *	KEY_WEBSITE = 3,
 *
 * When sending a message back to the Pebble C script, the object 
 * key must correspond to the key values above.
 *
 */
Pebble.addEventListener('appmessage', function(e) {
  console.log("appMessage Event");
  env  = setInStorage('env', e.payload.env, 0);
  when = setInStorage('when', findWhen(env), 'today');
	
  var post = "email="+encodeURIComponent(email)+"&password="+encodeURIComponent(password)+"&profile="+encodeURIComponent(profile)+"&website="+encodeURIComponent(website);
	
  sendRequest(url + 'pebble.php?when=' + when, post, function(response) {
    Pebble.sendAppMessage({ '1': displayHits(response.count), '2': response.when, '3': website }, pebbleMessageSuccess, pebbleMessageFailure);
  });
});

/**
 * Show Configuration Listener
 *
 * The event is fired when the user taps the cog icon in next to the
 * App name in the Pebble phone App.
 *
 * We use Pebble.openUrl() to pass URI encoded data to our 
 * configuration script.
 *
 */
Pebble.addEventListener("showConfiguration", function(e) {
  console.log("showConfiguration Event");

  email    = getInStorage('email', '');
  password = getInStorage('password', '');
  profile  = getInStorage('profile', '');
  website  = getInStorage('website', '');

  Pebble.openURL(url + "configuration.php?" + "email=" + encodeURIComponent(email) + "&password=" + encodeURIComponent(password) + "&profile=" + encodeURIComponent(profile));
});

/**
 * Web View Closed Listener
 *
 * The event is fired when the Pebble phone app receives a close
 * command via the PebbleJS protocol (pebblejs://close).
 *
 * Arguments can be passed to the listener as a URI encoded JSON
 * string and accessed by running JSON.parse on the e.response
 * object.
 */
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("webViewClosed Event");
	
  if (typeof e.response != 'undefined') {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    if (typeof configuration == 'object') {
      email    = setInStorage('email', configuration.email, '');
      password = setInStorage('password', configuration.password, '');
      profile  = setInStorage('profile', configuration.profile, '');
      website  = setInStorage('website', configuration.website, '');
      when     = getInStorage('when', 'today');
      var post =  "email="+encodeURIComponent(email)+"&password="+encodeURIComponent(password)+"&profile="+encodeURIComponent(profile)+"&website="+encodeURIComponent(website);

      sendRequest(url + 'pebble.php?when=' + when, post, function(response) {
        Pebble.sendAppMessage({ '1': displayHits(response.count), '2': response.when, '3': website }, pebbleMessageSuccess, pebbleMessageFailure);
      });
    }
  }
});
