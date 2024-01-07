/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.
*/

#ifndef __GTKMMUTILITYS_HPP__
#define __GTKMMUTILITYS_HPP__

#include <string>
#include <thread>
#include <iostream>

#include <gtkmm-3.0/gtkmm/hvbox.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/switch.h>
#include <gtkmm-3.0/gtkmm/dialog.h>
#include <glibmm-2.4/glibmm/dispatcher.h>
#include <gtkmm-3.0/gtkmm/messagedialog.h>


/// @brief cree un bouton de type switch avec sont label
class SwitchUser : public Gtk::HBox
{
   public :
      SwitchUser( bool _default = false , std::string const & _label ="");


      void set_label(std::string const & _label);
      bool get_active(void);
      void set_active(bool _stat);

     
       Glib::SignalProxyProperty  signal_changed(void);

   private:
      Gtk::Switch m_switchButton;
      Gtk::Label m_label;

} ;

/// @brief initialisse le bouton
/// @param _default etat par defaut
/// @param _label nom du bouton
SwitchUser::SwitchUser(bool _default  , std::string const & _label )
{
    this->pack_start( this->m_label , Gtk::PACK_SHRINK);
    this->pack_start( this->m_switchButton , Gtk::PACK_SHRINK );

    this->set_label(_label);
    this->set_active(_default);
}


/// @brief met a jour le nom du bouton
/// @param _label 
void SwitchUser::set_label(std::string const & _label)
{
    this->m_label.set_label(_label + " ");
}

/// @brief renvoie l'etat du bouton
bool SwitchUser::get_active(void)
{
    return this->m_switchButton.get_active();
}

/// @brief impose un etat au bouton
/// @param _stat etat a imposer
void SwitchUser::set_active(bool _stat)
{
    this->m_switchButton.set_active(_stat); 
    this->m_switchButton.property_active().signal_changed();
}

/// @brief renvoie le signal associer au changement d'etat
/// @return 
Glib::SignalProxyProperty  SwitchUser::signal_changed(void)
{
    return  this->m_switchButton.property_active().signal_changed();
}



/// @brief cree un dialog dans un thread séparé
class DialogThread : public Gtk::Dialog
{
   public :
      DialogThread(void);
      ~DialogThread(void);

      void run(void);
      void stop(void);

   private :

      bool thread_func(void);

       std::unique_ptr<std::jthread> m_th;
       std::atomic<bool> m_continue , m_end ;
       Glib::Dispatcher m_signalDone;

       std::chrono::_V2::steady_clock::time_point m_start;
      
};

/// @brief initialisation de la fenetre 
/// @param  
DialogThread::DialogThread(void):Gtk::Dialog()
{
    this->m_signalDone.connect(sigc::mem_fun(this, &DialogThread::show_all));
    this->m_continue = false;
    this->m_end = false;
    this->m_th = nullptr;
}

/// @brief arret de la fenetre et de son thread
/// @param  
DialogThread::~DialogThread(void)
{
    this->stop();
}

/// @brief demare le thread
/// @param  
void DialogThread::run(void)
{
    std::chrono::duration<double> elapsed_seconds{std::chrono::steady_clock::now() - this->m_start};

    if(this->m_continue || elapsed_seconds.count() < 500 )
        return ;

    this->m_end = false;
    this->m_continue = true;
    this->m_th.reset(new std::jthread(&DialogThread::thread_func, this)) ;
}

/// @brief arret du thread
/// @param  
void DialogThread::stop(void)
{
    if(!this->m_continue || !this->m_th )
        return;
    
    this->m_continue = false;
    if(this->m_th->joinable())
        this->m_th->join();

    
    this->m_th.reset();

    this->m_start = std::chrono::steady_clock::now();
}

/// @brief routine du thread
/// @param  
/// @return 
bool DialogThread::thread_func(void)
{
    while( this->m_continue || !this->m_th)
    {
        this->m_signalDone.emit();

        //emet un delai avant la mise a jour de la fenetre
        const std::chrono::duration<double, std::milli> duration(500);

        //s'assure que apres le delais on est tjr ouvert
        if(!this->m_continue)
            break;

        std::this_thread::sleep_for(duration);
    }

    this->m_end = true;
    return true;
}


/// @brief permet d'aficher dans une fenetre d'erreur une exception logic
class LogicExceptionDialog : public std::logic_error
{
    public : 
        LogicExceptionDialog(std::string const & _msg);

        void show(void) const;
};


LogicExceptionDialog::LogicExceptionDialog(std::string const & _msg) : std::logic_error(_msg)
{
    
}

void LogicExceptionDialog::show( void )const
{

    std::cerr << this->what() << std::endl;

    Gtk::MessageDialog dialog( "Erreur");

    dialog.set_secondary_text(this->what());

    dialog.run();
}

#endif