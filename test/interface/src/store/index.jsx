import composition from './composition'



function StoreStack({children, storeKeys})
{   
    if(!storeKeys.length)
        return children || null

    const Provider = composition[storeKeys[0]]
    return (
    <Provider key={storeKeys[0]}>
        <StoreStack storeKeys={storeKeys.slice(1)}>
            {children}
        </StoreStack>
    </Provider>
    )
}

export default function Store({children}){
    const storeKeys = Object.keys(composition)

    return <StoreStack storeKeys={storeKeys}>
        {children}
    </StoreStack>
}