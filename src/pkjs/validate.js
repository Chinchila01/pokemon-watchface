module.exports = function(minified) {
  var clayConfig = this;
  var _ = minified._;
  var $ = minified.$;
  var HTML = minified.HTML;

  function setSliderRange() {
    
    var value = this.get();
    if (value && value === '1') {
      clayConfig.getItemByMessageKey('hours_time').hide();
      clayConfig.getItemByMessageKey('minutes_time').show();
    } else {
      clayConfig.getItemByMessageKey('minutes_time').hide();
      clayConfig.getItemByMessageKey('hours_time').show();
    }
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    clayConfig.getItemByMessageKey('minutes_time').hide();
    var timeUnit = clayConfig.getItemByMessageKey('time_unit');
    setSliderRange.call(timeUnit);
    timeUnit.on( 'change', setSliderRange);
  });
  
};