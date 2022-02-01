export default {
    topTabs : (context) => {
        
        return context.getter('availTabs', context).filter(tab => tab.topTab && tab.topTab(context))
    },

    availTabs : (context) => {
        const availTabs = context.state.tabs.filter(tab => !("availTab" in tab) || tab.availTab(context))
        console.log('awailTabs:', availTabs)
        return availTabs
    },
    getMenuTabs : (context) => {
        return context.getter('availTabs', context).filter(tab => !tab.hidden || ! tab.hidden(context))
    },
    currentUser : ({getter}) => getter(['auth', 'currentUser']),
}