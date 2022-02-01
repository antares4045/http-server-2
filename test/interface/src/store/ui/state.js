export default {
    alertType : null,
    alertText : null,
    alertCloseOn : 0,

    openingTab : null,

    tabs : [
         { 
            id : 'startPage',
            route : '/',
            exactRoute : true,
            name : 'Стартовая страница',
            page : 'startPage',
            topTab : () => true,
            availTab : ({getter}) => true,
        },
        { 
            // Эта таба должна идти в списке последней!
            id : '404',

            route : '/',
            exactRoute : false,

            name : 'Данной станицы не существует',
            page : 'Page404',
            availTab : ({getter}) => true,
            hidden : () => true
        },
    ]
}