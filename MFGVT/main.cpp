/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.
*/


#include "includes/gtkmmcomponent.hpp"


int main(int argc, char *argv[]) 
{
    //séparte une chemin et son fichier
    // ici correspond au chemin absolut du programme 
    auto progmPath = File::sep_sub_and_name(std::string(argv[0]));

    Gtk::Main app(argc, argv);

    try
    {
        //fenetre principal avec chemin absolut partager
        WindowMain fenetre(std::get<0>(progmPath));

	    fenetre.set_title("Maintenance File Grouper Version Tool");

        //! charge les donné , doit etre fait avant WindowMain::init()
        // cherche les donné dans le chamin de Ressource
        auto tload = Device::load( fenetre.get_ressourcePath() );

        fenetre.addMainPath( std::shared_ptr< const std::string >(new const std::string(std::get<0>(tload))) );

        //ajoute tout les materiel charger dan s lezs donné a partagé
        fenetre.addDevice( std::get<1>(tload) );
        
        //partage le chemin principal a tout les materiel
        for(auto & dev : *fenetre.atDevice() )
            dev.addMainPath(fenetre.atMainPath());
        
        
        fenetre.init();
        
        fenetre.show_all();
        
        Gtk::Main::run(fenetre);
    }
    catch(LogicExceptionDialog const & _e)
    {
        _e.show();
        std::cin.get();
    }
    catch(std::exception const & _e)
    {
        std::cerr << _e.what() << std::endl;

        std::cin.get();
    }
    
    

    return 0;
}

