module.exports = function(minified) {
  var clayConfig = this;
  var _ = minified._;
  var $ = minified.$;
  var HTML = minified.HTML;

  function setSliderRange() {
    
    var value = this.get();
    var hoursItem = clayConfig.getItemByMessageKey('hours_value');
    var minutesItem = clayConfig.getItemByMessageKey('minutes_value');
    if (value && value === '1') { // Changed to minutes

      minutesItem.set(hoursItem.get());
      hoursItem.hide();
      minutesItem.show();
    } else { // Changed to hours
      hoursItem.set((minutesItem.get() > 24) ? 24 : minutesItem.get());
      minutesItem.hide();
      hoursItem.show();
    }
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var timeUnit = clayConfig.getItemByMessageKey('time_unit');
    setSliderRange.call(timeUnit);
    timeUnit.on('change', setSliderRange);
  });
  
};