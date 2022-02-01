const setter = (key) => ({state}, value) => state[key] = value

export default {
    SET : ({state}, {key, value}) => state[key] = value,

    raiseAlert : ({state}, {text, duration=1000, type="info"}) => {
        state.alertText = text
        state.alertCloseOn = duration < 0 ? -1 : +new Date() + duration
        state.alertType = type
    },
    closeAlert : ({state}) => {
        state.alertCloseOn = 0
    },
    setMenuOpened : setter('menuOpened'),
    toggleMenuOpened : ({state}) => state.menuOpened = !state.menuOpened,

    openTab : setter('openingTab'),
    
}

